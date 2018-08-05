/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumgizmo.cc
 *
 *  Thu Sep 16 10:24:40 CEST 2010
 *  Copyright 2010 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "drumgizmo.h"

#include <cmath>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <mutex>

#include <event.h>
#include <audiotypes.h>
#include <config.h>

#include <hugin.hpp>

#include "drumkitparser.h"
#include "audioinputenginemidi.h"

DrumGizmo::DrumGizmo(Settings& settings,
                     AudioOutputEngine& o, AudioInputEngine& i)
	: loader(settings, kit, i, resamplers, rand, audio_cache)
	, oe(o)
	, ie(i)
	, audio_cache(settings)
	, input_processor(settings, kit, activeevents, rand)
	, settings(settings)
	, settings_getter(settings)
{
	audio_cache.init(10000); // start thread
	events.reserve(1000);
	loader.init();
}

DrumGizmo::~DrumGizmo()
{
	loader.deinit();
	audio_cache.deinit(); // stop thread
}

bool DrumGizmo::init()
{
	if(!ie.init(kit.instruments))
	{
		return false;
	}

	if(!oe.init(kit.channels))
	{
		return false;
	}

	return true;
}

void DrumGizmo::setFrameSize(size_t framesize)
{
	settings.buffer_size.store(framesize);

	// If we are resampling override the frame size.
	if(resamplers.isActive() && enable_resampling)
	{
		framesize = RESAMPLER_INPUT_BUFFER;
	}

	if(this->framesize != framesize)
	{
		DEBUG(drumgizmo, "New framesize: %d\n", (int)framesize);

		this->framesize = framesize;

		// Update framesize in drumkitloader and cachemanager:
		loader.setFrameSize(framesize);
		audio_cache.setFrameSize(framesize);
	}
}

void DrumGizmo::setFreeWheel(bool freewheel)
{
	// Freewheel = true means that we are bouncing and therefore running faster
	// than realtime.
	this->freewheel = freewheel;
	audio_cache.setAsyncMode(!freewheel);
}

void DrumGizmo::setRandomSeed(unsigned int seed)
{
	rand.setSeed(seed);
}

bool DrumGizmo::run(size_t pos, sample_t *samples, size_t nsamples)
{
	if(settings_getter.enable_resampling.hasChanged())
	{
		enable_resampling = settings_getter.enable_resampling.getValue();
	}

	setFrameSize(nsamples);
	setFreeWheel(ie.isFreewheeling() && oe.isFreewheeling());

	ie.pre();
	oe.pre(nsamples); // Clears all output buffers
	std::memset(samples, 0, nsamples * sizeof(sample_t));

	//
	// Read new events
	//

	ie.run(pos, nsamples, events);

	double resample_ratio = resamplers.getRatio();
	if(enable_resampling == false)
	{
		resample_ratio = 1.0;
	}
	bool active_events_left =
		input_processor.process(events, pos, resample_ratio);

	if(!active_events_left)
	{
		return false;
	}

	events.clear();

	//
	// Write audio
	//
#ifdef WITH_RESAMPLER
	if(!enable_resampling || !resamplers.isActive()) // No resampling needed
	{
#endif
		for(size_t c = 0; c < kit.channels.size(); ++c)
		{
			sample_t *buf = samples;
			bool internal = false;
			if(oe.getBuffer(c))
			{
				buf = oe.getBuffer(c);
				internal = true;
			}

			if(buf)
			{
				std::memset(buf, 0, nsamples * sizeof(sample_t));

				getSamples(c, pos, buf, nsamples);

				if(!internal)
				{
					oe.run(c, samples, nsamples);
				}
			}
		}
#ifdef WITH_RESAMPLER
	}
	else
	{
		// Resampling needed

		//
		// NOTE: Channels must be processed one buffer at a time on all channels in
		// parallel - NOT all buffers on one channel and then all buffer on the next
		// one since this would mess up the event queue (it would jump back and
		// forth in time)
		//

		// Prepare output buffer
		for(size_t c = 0; c < kit.channels.size(); ++c)
		{
			resamplers[c].setOutputSamples(resampler_output_buffer[c], nsamples);
		}

		// Process channel data
		size_t kitpos = pos * resamplers.getRatio();
		size_t insize = sizeof(resampler_input_buffer[0]) / sizeof(sample_t);

		while(resamplers.getOutputSampleCount() > 0)
		{
			for(size_t c = 0; c < kit.channels.size(); ++c)
			{
				if(resamplers[c].getInputSampleCount() == 0)
				{
					sample_t *sin = resampler_input_buffer[c];
					memset(resampler_input_buffer[c], 0,
					       sizeof(resampler_input_buffer[c]));
					getSamples(c, kitpos, sin, insize);

					resamplers[c].setInputSamples(sin, insize);
				}
				resamplers[c].process();
			}
			kitpos += insize;
		}

		// Write output data to output engine.
		for(size_t c = 0; c < kit.channels.size(); ++c)
		{
			oe.run(c, resampler_output_buffer[c], nsamples);
		}

	}
#endif/*WITH_RESAMPLER*/

	ie.post();
	oe.post(nsamples);

	pos += nsamples;

	return true;
}

void DrumGizmo::renderSampleEvent(EventSample& evt, int pos, sample_t *s, std::size_t sz)
{
	size_t n = 0; // default start point is 0.

	// If we are not at offset 0 in current buffer:
	if(evt.offset > (size_t)pos)
	{
		n = evt.offset - pos;
	}

	size_t end = sz; // default end point is the end of the buffer.

	// Find the end point intra-buffer
	if((evt.t + end - n) > evt.sample_size)
	{
		end = evt.sample_size - evt.t + n;
	}

	// This should not be necessary but make absolutely sure that we do
	// not write over the end of the buffer.
	if(end > sz)
	{
		end = sz;
	}

	size_t t = 0; // Internal buffer counter

repeat:
	float scale = 1.0f;
	for(; (n < end) && (t < (evt.buffer_size - evt.buffer_ptr)); ++n)
	{
		assert(n >= 0);
		assert(n < sz);

		assert(t >= 0);
		assert(t < evt.buffer_size - evt.buffer_ptr);

		if(evt.rampdownInProgress() && evt.rampdown_offset < (evt.t + t) &&
		   evt.rampdown_count > 0)
		{
			scale = std::min((float)evt.rampdown_count / evt.ramp_length, 1.f);
			evt.rampdown_count--;
		}

		s[n] += evt.buffer[evt.buffer_ptr + t] * evt.scale * scale;
		++t;
	}

	// Add internal buffer counter to "global" event counter.
	evt.t += t;//evt.buffer_size;
	evt.buffer_ptr += t;

	if(n != sz && evt.t < evt.sample_size)
	{
		evt.buffer_size = sz - n;// Hint new size

		// More samples needed for current buffer
		evt.buffer = audio_cache.next(evt.cache_id, evt.buffer_size);

		evt.buffer_ptr = 0;
		t = 0;
		goto repeat;
	}
}

void DrumGizmo::getSamples(int ch, int pos, sample_t* s, size_t sz)
{
	// Store local values of settings to ensure they don't change intra-iteration
	const auto enable_bleed_control = settings.enable_bleed_control.load();
	const auto master_bleed = settings.master_bleed.load();

	std::vector< Event* > erase_list;
	std::list< Event* >::iterator i = activeevents[ch].begin();
	for(; i != activeevents[ch].end(); ++i)
	{
		bool removeevent = false;

		Event* event = *i;
		Event::type_t type = event->getType();
		switch(type)
		{
		case Event::sample:
			{
				EventSample& evt = *static_cast<EventSample*>(event);
				AudioFile& af = *evt.file;

				if(!af.isLoaded() || !af.isValid() || (s == nullptr))
				{
					removeevent = true;
					break;
				}

				if(evt.offset > (pos + sz))
				{
					// Don't handle event now. It is scheduled for a future iteration.
					continue;
				}

				if(evt.cache_id == CACHE_NOID)
				{
					size_t initial_chunksize = (pos + sz) - evt.offset;
					evt.buffer = audio_cache.open(af, initial_chunksize,
					                              af.filechannel, evt.cache_id);
					if((af.mainState() == main_state_t::is_not_main) &&
					   enable_bleed_control)
					{
						evt.scale = master_bleed;
					}

					evt.buffer_size = initial_chunksize;
					evt.sample_size = af.size;
				}

				{
					std::lock_guard<std::mutex> guard(af.mutex);

					renderSampleEvent(evt, pos, s, sz);

					if((evt.t >= evt.sample_size) || (evt.rampdown_count == 0))
					{
						removeevent = true;
					}

					if(evt.buffer_ptr >= evt.buffer_size && removeevent == false)
					{
						evt.buffer_size = sz;
						evt.buffer = audio_cache.next(evt.cache_id, evt.buffer_size);
						evt.buffer_ptr = 0;
					}

					if(removeevent)
					{
						audio_cache.close(evt.cache_id);
					}
				}
			}
			break;
		}

		if(removeevent)
		{
			erase_list.push_back(event); // don't delete until we are out of the loop.
			continue;
		}
	}

	for(auto& event : erase_list)
	{
		activeevents[ch].remove(event);
		delete event;
	}
}

void DrumGizmo::stop()
{
	// engine.stop();
}

std::size_t DrumGizmo::getLatency() const
{
	auto latency = input_processor.getLatency();
	if(enable_resampling)
	{
		latency += resamplers.getLatency();
	}

	return latency;
}

int DrumGizmo::samplerate()
{
	return settings.samplerate.load();
}

void DrumGizmo::setSamplerate(int samplerate)
{
	DEBUG(dgeditor, "%s samplerate: %d\n", __PRETTY_FUNCTION__, samplerate);
	settings.samplerate.store(samplerate);

	// Notify input engine of the samplerate change.
	ie.setSampleRate(samplerate);

#ifdef WITH_RESAMPLER
	resamplers.setup(kit.getSamplerate(), settings.samplerate.load());
#endif/*WITH_RESAMPLER*/
}

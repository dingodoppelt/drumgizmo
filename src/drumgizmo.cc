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

#include "audioinputenginemidi.h"

DrumGizmo::DrumGizmo(Settings& settings,
                     AudioOutputEngine& o, AudioInputEngine& i)
	: loader(settings, kit, i, rand, audio_cache)
	, oe(o)
	, ie(i)
	, audio_cache(settings)
	, input_processor(settings, kit, events_ds, rand)
	, settings(settings)
	, settings_getter(settings)
{
	audio_cache.init(10000); // start thread
	events.reserve(1000);
	loader.init();
	setSamplerate(44100.0f, settings.resampling_quality.load());
	settings_getter.audition_counter.hasChanged(); // Reset audition_counter

	config.load();
	if(config.defaultMidimap != "" && settings.midimap_file.load() == "")
	{
		settings.midimap_file.store(config.defaultMidimap);
	}
	if(config.defaultKit != "" && settings.drumkit_file.load() == "")
	{
		settings.drumkit_file.store(config.defaultKit);
	}
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
	if(settings_getter.drumkit_file.hasChanged())
	{
		// New kit loaded/loading - old events no longer makes sense.
		events_ds.clear();
	}

	if(settings_getter.enable_resampling.hasChanged())
	{
		enable_resampling = settings_getter.enable_resampling.getValue();
	}

	{
		auto sample_rate_changed = settings_getter.drumkit_samplerate.hasChanged();
		auto resampling_quality_changed = settings_getter.resampling_quality.hasChanged();
		if(sample_rate_changed || resampling_quality_changed)
		{
			settings_getter.drumkit_samplerate.getValue(); // stage new value
			setSamplerate(settings.samplerate.load(),
			              settings_getter.resampling_quality.getValue());
		}
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

	double resample_ratio = ratio;
	if(enable_resampling == false)
	{
		resample_ratio = 1.0;
	}

	if(settings_getter.audition_counter.hasChanged())
	{
		settings_getter.audition_counter.getValue();
		auto instrument_name = settings.audition_instrument.load();
		auto velocity = settings.audition_velocity.load();

		std::size_t instrument_index = 0;
		for (std::size_t i = 0; i < kit.instruments.size(); ++i)
		{
			if (instrument_name == kit.instruments[i]->getName())
			{
				instrument_index = i;
			}
		}

		events.push_back({EventType::OnSet, instrument_index, 0, velocity});
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
	if(!enable_resampling || ratio == 1.0)
	{
		// No resampling needed

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
	}
	else
	{
		// Resampling needed
		size_t kitpos = pos * ratio;
		for(size_t c = 0; c < kit.channels.size(); ++c)
		{
			sample_t *buf = samples;
			bool internal = false;
			if(oe.getBuffer(c))
			{
				buf = oe.getBuffer(c);
				internal = true;
			}

			zita[c].set_out_data(buf);
			zita[c].set_out_count(nsamples);

			if(zita[c].get_inp_count() > 0)
			{
				// Samples left from last iteration, process that one first
				zita[c].process();
			}

			std::memset(resampler_input_buffer[c].get(), 0, MAX_RESAMPLER_BUFFER_SIZE);

			zita[c].set_inp_data(resampler_input_buffer[c].get());
			std::size_t sample_count =
				std::ceil((nsamples - (nsamples - zita[c].get_out_count())) * ratio);
			getSamples(c, kitpos, zita[c].get_inp_data(), sample_count);

			zita[c].set_inp_count(sample_count);
			zita[c].process();
			if(!internal)
			{
				oe.run(c, samples, nsamples);
			}
		}
	}

	ie.post();
	oe.post(nsamples);

	pos += nsamples;

	return true;
}

void DrumGizmo::renderSampleEvent(SampleEvent& evt, int pos, sample_t *s, std::size_t sz)
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
			if(evt.ramp_length > 0)
			{
				scale = std::min((float)evt.rampdown_count / evt.ramp_length, 1.f);
			}
			else
			{
				scale = 0.0f;
			}
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

	EventIDs to_remove;
	for(auto& sample_event : events_ds.iterateOver<SampleEvent>(ch))
	{
		bool removeevent = false;

		AudioFile& af = *sample_event.file;

		if(!af.isLoaded() || !af.isValid() || (s == nullptr))
		{
			// This event cannot be played - schedule for deletion and continue.
			to_remove.push_back(sample_event.id);
			continue;
		}

		if(sample_event.offset > (pos + sz))
		{
			// Don't handle event now. It is scheduled for a future iteration.
			continue;
		}

		if(sample_event.cache_id == CACHE_NOID)
		{
			size_t initial_chunksize = (pos + sz) - sample_event.offset;
			sample_event.buffer = audio_cache.open(af, initial_chunksize,
										  af.filechannel, sample_event.cache_id);
			if((af.mainState() == main_state_t::is_not_main) &&
			   enable_bleed_control)
			{
				sample_event.scale *= master_bleed;
			}

			sample_event.buffer_size = initial_chunksize;
			sample_event.sample_size = af.size;
		}

		{
			// TODO: We should make all audiofiles reference counted and get rid of this lock.
			std::lock_guard<std::mutex> guard(af.mutex);

			renderSampleEvent(sample_event, pos, s, sz);

			if((sample_event.t >= sample_event.sample_size) || (sample_event.rampdown_count == 0))
			{
				removeevent = true;
			}

			if(sample_event.buffer_ptr >= sample_event.buffer_size && !removeevent)
			{
				sample_event.buffer_size = sz;
				sample_event.buffer = audio_cache.next(sample_event.cache_id, sample_event.buffer_size);
				sample_event.buffer_ptr = 0;
			}

			if(removeevent)
			{
				audio_cache.close(sample_event.cache_id);
			}
		}

		if(removeevent)
		{
			to_remove.push_back(sample_event.id); // don't delete until we are out of the loop.
		}
	}

	for(auto event_id : to_remove)
	{
		events_ds.remove(event_id);
	}
}

void DrumGizmo::stop()
{
	// engine.stop();
}

std::size_t DrumGizmo::getLatency() const
{
	auto latency = input_processor.getLatency();
	if(enable_resampling && ratio != 0.0)
	{
		// TODO:
		latency += zita[0].inpsize(); // resampler latency
	}

	return latency;
}

float DrumGizmo::samplerate()
{
	return settings.samplerate.load();
}

void DrumGizmo::setSamplerate(float samplerate, float resampling_quality)
{
	DEBUG(dgeditor, "%s samplerate: %f\n", __PRETTY_FUNCTION__, samplerate);
	settings.samplerate.store(samplerate);

	// Notify input engine of the samplerate change.
	ie.setSampleRate(samplerate);

	auto input_fs = settings.drumkit_samplerate.load();
	auto output_fs = samplerate;
	ratio = input_fs / output_fs;
	settings.resampling_recommended.store(ratio != 1.0);

	// TODO: Only reallocate the actual amount of samples needed based on the
	// ratio and the framesize.
	for(auto& buf : resampler_input_buffer)
	{
		buf.reset(new sample_t[MAX_RESAMPLER_BUFFER_SIZE]);
	}

	for(int c = 0; c < NUM_CHANNELS; ++c)
	{
		zita[c].reset();
		auto nchan = 1u; // mono
		// 16 ≤ hlen ≤ 96 - default is 72, q: 0.7f
		resampling_quality = std::max(0.0f, std::min(1.0f, resampling_quality));
		std::size_t hlen = 16 + (96 - 16) * resampling_quality;
		zita[c].setup(input_fs, output_fs, nchan, hlen);

		// Prefill
		auto null_size = zita[c].inpsize() - 1;// / 2 - 1;
		zita[c].set_inp_data(nullptr);
		zita[c].set_inp_count(null_size);

		auto scratch_buffer_size = (null_size / ratio) + 1;
		if(scratch_buffer.size() < scratch_buffer_size)
		{
			scratch_buffer.resize(scratch_buffer_size);
		}
		zita[c].set_out_data(scratch_buffer.data());
		zita[c].set_out_count(scratch_buffer_size);

		zita[c].process();
	}
}

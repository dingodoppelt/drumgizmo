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

#include <math.h>
#include <stdio.h>
#include <assert.h>

#include <event.h>
#include <audiotypes.h>

#include <string.h>

#include <hugin.hpp>

#include <config.h>
#include <memory>

#include <iostream>

#include "drumkitparser.h"
#include "audioinputenginemidi.h"
#include "configparser.h"

#include "nolocale.h"

DrumGizmo::DrumGizmo(Settings& settings,
                     AudioOutputEngine& o, AudioInputEngine& i)
	: loader(settings, kit, i, resamplers, rand)
	, oe(o)
	, ie(i)
	, kit()
	, input_processor(kit, activeevents)
	, framesize(0)
	, freewheel(false)
	, events{}
	, settings(settings)
{
	audioCache.init(10000); // start thread
	events.reserve(1000);
	loader.init();
}

DrumGizmo::~DrumGizmo()
{
	loader.deinit();
	audioCache.deinit(); // stop thread
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
	// If we are resampling override the frame size.
	if(resamplers.isActive())
	{
		framesize = RESAMPLER_INPUT_BUFFER;
	}

	if(this->framesize != framesize)
	{
		DEBUG(drumgizmo, "New framesize: %d\n", (int)framesize);

		this->framesize = framesize;

		// Update framesize in drumkitloader and cachemanager:
		loader.setFrameSize(framesize);
		audioCache.setFrameSize(framesize);
	}
}

void DrumGizmo::setFreeWheel(bool freewheel)
{
	// Freewheel = true means that we are bouncing and therefore running faster
	// than realtime.
	if(freewheel != this->freewheel)
	{
		this->freewheel = freewheel;
		audioCache.setAsyncMode(!freewheel);
	}
}

void DrumGizmo::setRandomSeed(unsigned int seed)
{
	rand.setSeed(seed);
}

void DrumGizmo::run(int endpos)
{
	size_t pos = 0;
	size_t nsamples = oe.getBufferSize();
	sample_t *samples = (sample_t *)malloc(nsamples * sizeof(sample_t));

	setFrameSize(oe.getBufferSize());

	ie.start();
	oe.start();

	while(run(pos, samples, nsamples) == true)
	{
		pos += nsamples;
		if((endpos != -1) && (pos >= (size_t)endpos))
		{
			break;
		}
	}

	ie.stop();
	oe.stop();

	free(samples);
}

bool DrumGizmo::run(size_t pos, sample_t *samples, size_t nsamples)
{
	setFrameSize(nsamples);

	ie.pre();
	oe.pre(nsamples);

	//
	// Read new events
	//

	ie.run(pos, nsamples, events);

	double resample_ratio = resamplers.getRatio();
	bool active_events_left = input_processor.process(events, pos, resample_ratio);

	if(!active_events_left)
	{
		return false;
	}
	events.clear();

	//
	// Write audio
	//
#ifdef WITH_RESAMPLER
	if((settings.enable_resampling.load() == false) ||
	   (!resamplers.isActive())) // No resampling needed
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
				memset(buf, 0, nsamples * sizeof(sample_t));

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

#undef SSE // SSE broken for now ... so disable it.
#ifdef SSE
#define N 8
typedef float vNsf __attribute__ ((vector_size(sizeof(sample_t)*N)));
#endif/*SSE*/

void DrumGizmo::getSamples(int ch, int pos, sample_t* s, size_t sz)
{
	std::vector< Event* > erase_list;
	std::list< Event* >::iterator i = activeevents[ch].begin();
	for(; i != activeevents[ch].end(); ++i)
	{
		bool removeevent = false;

		Event* event = *i;

		Event::type_t type = event->getType();
		switch(type) {
		case Event::sample:
			{
				EventSample& evt = *static_cast<EventSample*>(event);
				AudioFile& af = *evt.file;

				if(!af.isLoaded() || !af.isValid() || (s == nullptr))
				{
					removeevent = true;
					break;
				}

				// Don't handle event now is is scheduled for a future iteration?
				if(evt.offset > (pos + sz))
				{
					continue;
				}

				if(evt.cache_id == CACHE_NOID)
				{
					size_t initial_chunksize = (pos + sz) - evt.offset;
					evt.buffer = audioCache.open(af, initial_chunksize,
					                             af.filechannel, evt.cache_id);
					evt.buffer_size = initial_chunksize;
				}

				{
					MutexAutolock l(af.mutex);

					size_t n = 0; // default start point is 0.

					// If we are not at offset 0 in current buffer:
					if(evt.offset > (size_t)pos)
					{
						n = evt.offset - pos;
					}

					size_t end = sz; // default end point is the end of the buffer.

					// Find the end point intra-buffer
					if((evt.t + end - n) > af.size)
					{
						end = af.size - evt.t + n;
					}

					// This should not be necessary but make absolutely sure that we do
					// not write over the end of the buffer.
					if(end > sz)
					{
						end = sz;
					}

					size_t t = 0; // Internal buffer counter
					if(evt.rampdown == NO_RAMPDOWN)
					{

#ifdef SSE
						size_t optend = ((end - n) / N) * N + n;

						// Force source addr to be 16 byte aligned...
						// (might skip 1 or 2 samples)
						while((size_t)&evt.buffer[t] % 16)
						{
							++t;
						}

						for(; (n < optend) && (t < evt.buffer_size); n += N)
						{
							*(vNsf*)&(s[n]) += *(vNsf*)&(evt.buffer[t]);
							t += N;
						}
#endif
						for(; (n < end) && (t < evt.buffer_size); ++n)
						{
							assert(n >= 0);
							assert(n < sz);

							assert(t >= 0);
							assert(t < evt.buffer_size);

							s[n] += evt.buffer[t];
							++t;
						}
					}
					else
					{ // Ramp down in progress.
						for(; (n < end) && (t < evt.buffer_size) && evt.rampdown; ++n)
						{
							float scale = (float)evt.rampdown/(float)evt.ramp_start;
							s[n] += evt.buffer[t] * scale;
							++t;
							evt.rampdown--;
						}
					}

					// Add internal buffer counter to "global" event counter.
					evt.t += evt.buffer_size;

					if((evt.t < af.size) && (evt.rampdown != 0))
					{
						evt.buffer = audioCache.next(evt.cache_id, evt.buffer_size);
					}
					else
					{
						removeevent = true;
					}

					if(removeevent)
					{
						audioCache.close(evt.cache_id);
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
	return input_processor.getLatency() + resamplers.getLatency();
}

int DrumGizmo::samplerate()
{
	return settings.samplerate.load();
}

void DrumGizmo::setSamplerate(int samplerate)
{
	DEBUG(dgeditor, "%s samplerate: %d\n", __PRETTY_FUNCTION__, samplerate);
	settings.samplerate.store(samplerate);
#ifdef WITH_RESAMPLER
	resamplers.setup(kit.getSamplerate(), settings.samplerate.load());

	if(resamplers.isActive())
	{
		setFrameSize(RESAMPLER_INPUT_BUFFER);
	}
#endif/*WITH_RESAMPLER*/
}

std::string float2str(float a)
{
	char buf[256];
	snprintf_nol(buf, sizeof(buf) - 1, "%f", a);
	return buf;
}

std::string bool2str(bool a)
{
	return a?"true":"false";
}

float str2float(std::string a)
{
	if(a == "")
	{
		return 0.0;
	}

	return atof_nol(a.c_str());
}

std::string DrumGizmo::configString()
{
	std::string mmapfile;
	auto midiEngine = dynamic_cast<AudioInputEngineMidi*>(&ie);
	if(midiEngine)
	{
		mmapfile = midiEngine->getMidimapFile();
	}

	return
		"<config>\n"
		"  <value name=\"drumkitfile\">" + kit.getFile() + "</value>\n"
		"  <value name=\"midimapfile\">" + mmapfile + "</value>\n"
		"  <value name=\"enable_velocity_modifier\">" +
		bool2str(settings.enable_velocity_modifier.load()) + "</value>\n"
		"  <value name=\"velocity_modifier_falloff\">" +
		float2str(settings.velocity_modifier_falloff.load()) + "</value>\n"
		"  <value name=\"velocity_modifier_weight\">" +
		float2str(settings.velocity_modifier_weight.load()) + "</value>\n"
		"  <value name=\"enable_velocity_randomiser\">" +
		bool2str(settings.enable_velocity_randomiser.load()) + "</value>\n"
		"  <value name=\"velocity_randomiser_weight\">" +
		float2str(settings.velocity_randomiser_weight.load()) + "</value>\n"
		"</config>";
}

bool DrumGizmo::setConfigString(std::string cfg)
{
	DEBUG(config, "Load config: %s\n", cfg.c_str());

	std::string dkf;
	ConfigParser p;
	if(p.parseString(cfg))
	{
		ERR(drumgizmo, "Config parse error.\n");
		return false;
	}

	if(p.value("enable_velocity_modifier") != "")
	{
		settings.enable_velocity_modifier.store(p.value("enable_velocity_modifier") == "true");
	}

	if(p.value("velocity_modifier_falloff") != "")
	{
		settings.velocity_modifier_falloff.store(str2float(p.value("velocity_modifier_falloff")));
	}

	if(p.value("velocity_modifier_weight") != "")
	{
		settings.velocity_modifier_weight.store(str2float(p.value("velocity_modifier_weight")));
	}

	if(p.value("enable_velocity_randomiser") != "")
	{
		settings.enable_velocity_randomiser.store(p.value("enable_velocity_randomiser") == "true");
	}

	if(p.value("velocity_randomiser_weight") != "")
	{
		settings.velocity_randomiser_weight.store(str2float(p.value("velocity_randomiser_weight")));
	}

	if(p.value("enable_resampling") != "")
	{
		settings.enable_resampling.store(p.value("enable_resampling") == "true");
	}

	std::string newkit = p.value("drumkitfile");
	if(newkit != "")
	{
		settings.drumkit_file.store(newkit);
	}

	std::string newmidimap = p.value("midimapfile");
	if(newmidimap != "")
	{
		settings.midimap_file.store(newmidimap);
	}

	return true;
}

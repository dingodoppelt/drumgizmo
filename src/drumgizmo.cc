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
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "drumgizmo.h"

#include <math.h>
#include <stdio.h>

#include <event.h>
#include <audiotypes.h>

#include <string.h>

#include <hugin.hpp>

#include <config.h>

#include "drumkitparser.h"
#include "audioinputenginemidi.h"
#include "configuration.h"
#include "configparser.h"

#include "nolocale.h"

DrumGizmo::DrumGizmo(AudioOutputEngine *o, AudioInputEngine *i)
	: MessageReceiver(MSGRCV_ENGINE)
	, loader()
	, oe(o)
	, ie(i)
	, framesize(0)
	, freewheel(false)
{
	is_stopping = false;
	audioCache.init(1000); // start thread
}

DrumGizmo::~DrumGizmo()
{
	audioCache.deinit(); // stop thread
}

bool DrumGizmo::loadkit(std::string file)
{
	if(file == "")
	{
		return 1;
	}

	DEBUG(drumgizmo, "loadkit(%s)\n", file.c_str());

	// Remove all queue AudioFiles from loader before we actually delete them.
	loader.skip();

	// Delete all Channels, Instruments, Samples and AudioFiles.
	kit.clear();

	DrumKitParser parser(file, kit);
	if(parser.parse())
	{
		ERR(drumgizmo, "Drumkit parser failed: %s\n", file.c_str());
		return false;
	}

	loader.loadKit(&kit);

#ifdef WITH_RESAMPLER
	for(int i = 0; i < MAX_NUM_CHANNELS; ++i)
	{
		resampler[i].setup(kit.samplerate(), Conf::samplerate);
	}
#endif/*WITH_RESAMPLER*/

	DEBUG(loadkit, "loadkit: Success\n");

	return true;
}

bool DrumGizmo::init()
{
	if(!ie->init(kit.instruments))
	{
		return false;
	}

	if(!oe->init(kit.channels))
	{
		return false;
	}

	return true;
}

void DrumGizmo::handleMessage(Message *msg)
{
	DEBUG(msg, "got message.");
	switch(msg->type()) {
	case Message::LoadDrumKit:
		{
			DEBUG(msg, "got LoadDrumKitMessage message.");
			LoadDrumKitMessage *m = (LoadDrumKitMessage*)msg;
			loadkit(m->drumkitfile);
			//init(true);
		}
		break;
	case Message::LoadMidimap:
		DEBUG(msg, "got LoadMidimapMessage message.");
		if(!ie->isMidiEngine())
		{
			break;
		}
		{
			AudioInputEngineMidi *aim = (AudioInputEngineMidi*)ie;
			LoadMidimapMessage *m = (LoadMidimapMessage*)msg;
			bool ret = aim->loadMidiMap(m->midimapfile, kit.instruments);

			LoadStatusMessageMidimap *ls = new LoadStatusMessageMidimap();
			ls->success = ret;
			msghandler.sendMessage(MSGRCV_UI, ls);
		}
		break;
	case Message::EngineSettingsMessage:
		{
			bool mmap_loaded = false;
			std::string mmapfile;
			if(ie->isMidiEngine())
			{
				AudioInputEngineMidi *aim = (AudioInputEngineMidi*)ie;
				mmapfile = aim->midimapFile();
				mmap_loaded = aim->isValid();
			}

			EngineSettingsMessage *msg = new EngineSettingsMessage();
			msg->midimapfile = mmapfile;
			msg->midimap_loaded = mmap_loaded;
			msg->drumkitfile = kit.file();
			msg->drumkit_loaded = loader.isDone();
			msg->enable_velocity_modifier = Conf::enable_velocity_modifier;
			msg->velocity_modifier_falloff = Conf::velocity_modifier_falloff;
			msg->velocity_modifier_weight = Conf::velocity_modifier_weight;
			msg->enable_velocity_randomiser = Conf::enable_velocity_randomiser;
			msg->velocity_randomiser_weight = Conf::velocity_randomiser_weight;
			msghandler.sendMessage(MSGRCV_UI, msg);
		}
		break;
	case Message::ChangeSettingMessage:
		{
			ChangeSettingMessage *ch = (ChangeSettingMessage*)msg;
			switch(ch->name) {
			case ChangeSettingMessage::enable_velocity_modifier:
				Conf::enable_velocity_modifier = ch->value;
				break;
			case ChangeSettingMessage::velocity_modifier_weight:
				Conf::velocity_modifier_weight = ch->value;
				break;
			case ChangeSettingMessage::velocity_modifier_falloff:
				Conf::velocity_modifier_falloff = ch->value;
				break;
			}
		}
		break;
	default:
		break;
	}
}

void DrumGizmo::setFrameSize(size_t framesize)
{
	// If we are resampling override the frame size.
	if(resampler[0].ratio() != 1)
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

void DrumGizmo::run(int endpos)
{
	size_t pos = 0;
	size_t nsamples = oe->getBufferSize();
	sample_t *samples = (sample_t *)malloc(nsamples * sizeof(sample_t));

	setFrameSize(oe->getBufferSize());

	ie->start();
	oe->start();

	while(run(pos, samples, nsamples) == true)
	{
		pos += nsamples;
		if((endpos != -1) && (pos >= (size_t)endpos))
		{
			break;
		}
	}

	ie->stop();
	oe->stop();

	free(samples);
}

bool DrumGizmo::run(size_t pos, sample_t *samples, size_t nsamples)
{
	setFrameSize(nsamples);

	// Handle engine messages, at most one in each iteration:
	handleMessages(1);

	ie->pre();
	oe->pre(nsamples);

	//
	// Read new events
	//

	//DEBUG(engine, "Number of active events: %d\n", activeevents[0].size());

	size_t nev;
	event_t *evs = ie->run(pos, nsamples, &nev);

	for(size_t e = 0; e < nev; ++e)
	{
		if(evs[e].type == TYPE_ONSET)
		{
			Instrument *i = nullptr;
			int d = evs[e].instrument;
			/*
			  Instruments::iterator it = kit.instruments.begin();
			  while(d-- && it != kit.instruments.end())
			  {
			  i = &(it->second);
			  ++it;
			  }
			*/

			if(!kit.isValid())
			{
				continue;
			}

			if(d < (int)kit.instruments.size())
			{
				i = kit.instruments[d];
			}

			if(i == nullptr || !i->isValid())
			{
				ERR(drumgizmo, "Missing Instrument %d.\n", evs[e].instrument);
				continue;
			}

			if(i->group() != "")
			{
				// Add event to ramp down all existing events with the same groupname.
				Channels::iterator j = kit.channels.begin();
				while(j != kit.channels.end())
				{
					Channel &ch = *j;
					std::list< Event* >::iterator evs = activeevents[ch.num].begin();
					while(evs != activeevents[ch.num].end())
					{
						Event *ev = *evs;
						if(ev->type() == Event::sample)
						{
							EventSample *sev = (EventSample*)ev;
							if(sev->group == i->group() && sev->instrument != i)
							{
								sev->rampdown = 3000; // Ramp down 3000 samples
								// TODO: This must be configurable at some point...
								// ... perhaps even by instrument (ie. in the xml file)
								sev->ramp_start = sev->rampdown;
							}
						}
						++evs;
					}
					++j;
				}
			}

			Sample *s = i->sample(evs[e].velocity, evs[e].offset + pos);

			if(s == nullptr)
			{
				ERR(drumgizmo, "Missing Sample.\n");
				continue;
			}

			Channels::iterator j = kit.channels.begin();
			while(j != kit.channels.end())
			{
				Channel &ch = *j;
				AudioFile *af = s->getAudioFile(&ch);
				if(af)
				{
					// LAZYLOAD:
					// DEBUG(drumgizmo,"Requesting preparing of audio file\n");
					// loader.prepare(af);
				}
				if(af == nullptr || !af->isValid())
				{
					//DEBUG(drumgizmo,"Missing AudioFile.\n");
				}
				else
				{
					//DEBUG(drumgizmo, "Adding event %d.\n", evs[e].offset);
					Event *evt = new EventSample(ch.num, 1.0, af, i->group(), i);
					evt->offset = (evs[e].offset + pos) * resampler[0].ratio();
					activeevents[ch.num].push_back(evt);
				}
				++j;
			}
		}

		if(evs[e].type == TYPE_STOP)
		{
			is_stopping = true;
		}

		if(is_stopping)
		{
			// Count the number of active events.
			int num_active_events = 0;
			Channels::iterator j = kit.channels.begin();
			while(j != kit.channels.end())
			{
				Channel &ch = *j;
				num_active_events += activeevents[ch.num].size();
				++j;
			}

			if(num_active_events == 0)
			{
				// No more active events - now we can stop the engine.
				return false;
			}
		}

	}

	free(evs);

	//
	// Write audio
	//
#ifdef WITH_RESAMPLER
	if((Conf::enable_resampling == false) ||
	   (resampler[0].ratio() == 1.0)) // No resampling needed
	{
#endif
		for(size_t c = 0; c < kit.channels.size(); ++c)
		{
			sample_t *buf = samples;
			bool internal = false;
			if(oe->getBuffer(c))
			{
				buf = oe->getBuffer(c);
				internal = true;
			}

			if(buf)
			{
				memset(buf, 0, nsamples * sizeof(sample_t));

				getSamples(c, pos, buf, nsamples);

				if(!internal)
				{
					oe->run(c, samples, nsamples);
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
			resampler[c].setOutputSamples(resampler_output_buffer[c], nsamples);
		}

		// Process channel data
		size_t kitpos = pos * resampler[0].ratio();
		size_t insize = sizeof(resampler_input_buffer[0]) / sizeof(sample_t);

		while(resampler[0].getOutputSampleCount() > 0)
		{
			for(size_t c = 0; c < kit.channels.size(); ++c)
			{
				if(resampler[c].getInputSampleCount() == 0)
				{
					sample_t *sin = resampler_input_buffer[c];
					memset(resampler_input_buffer[c], 0,
					       sizeof(resampler_input_buffer[c]));
					getSamples(c, kitpos, sin, insize);

					resampler[c].setInputSamples(sin, insize);
				}
				resampler[c].process();
			}
			kitpos += insize;
		}

		// Write output data to output engine.
		for(size_t c = 0; c < kit.channels.size(); ++c)
		{
			oe->run(c, resampler_output_buffer[c], nsamples);
		}

	}
#endif/*WITH_RESAMPLER*/

	ie->post();
	oe->post(nsamples);

	pos += nsamples;

	return true;
}

#undef SSE // SSE broken for now ... so disable it.
#ifdef SSE
#define N 8
typedef float vNsf __attribute__ ((vector_size(sizeof(sample_t)*N)));
#endif/*SSE*/

void DrumGizmo::getSamples(int ch, int pos, sample_t *s, size_t sz)
{
	std::list< Event* >::iterator i = activeevents[ch].begin();
	for(; i != activeevents[ch].end(); ++i)
	{
		bool removeevent = false;

		Event *event = *i;

		Event::type_t type = event->type();
		switch(type) {
		case Event::sample:
			{
				EventSample *evt = (EventSample *)event;
				AudioFile *af = evt->file;

				if(!af->isLoaded() || !af->isValid() || (s == nullptr))
				{
					removeevent = true;
					break;
				}

				// Don't handle event now is is scheduled for a future iteration?
				if(evt->offset > (pos + sz))
				{
					continue;
				}

				if(evt->cache_id == CACHE_NOID)
				{
					size_t initial_chunksize = (pos + sz) - evt->offset;
					evt->buffer = audioCache.open(af, initial_chunksize,
					                              af->filechannel, evt->cache_id);
					evt->buffer_size = initial_chunksize;
				}

				{
					MutexAutolock l(af->mutex);

					size_t n = 0; // default start point is 0.

					// If we are not at offset 0 in current buffer:
					if(evt->offset > (size_t)pos)
					{
						n = evt->offset - pos;
					}

					size_t end = sz; // default end point is the end of the buffer.

					// Find the end point intra-buffer
					if((evt->t + end - n) > af->size)
					{
						end = af->size - evt->t + n;
					}

					// This should not be necessary but make absolutely sure that we do
					// not write over the end of the buffer.
					if(end > sz)
					{
						end = sz;
					}

					size_t t = 0; // Internal buffer counter
					if(evt->rampdown == NO_RAMPDOWN)
					{

#ifdef SSE
						size_t optend = ((end - n) / N) * N + n;

						// Force source addr to be 16 byte aligned...
						// (might skip 1 or 2 samples)
						while((size_t)&evt->buffer[t] % 16)
						{
							++t;
						}

						for(; (n < optend) && (t < evt->buffer_size); n += N)
						{
							*(vNsf*)&(s[n]) += *(vNsf*)&(evt->buffer[t]);
							t += N;
						}
#endif
						for(; (n < end) && (t < evt->buffer_size); ++n)
						{
							s[n] += evt->buffer[t];
							++t;
						}
					}
					else
					{ // Ramp down in progress.
						for(; (n < end) && (t < evt->buffer_size) && evt->rampdown; ++n)
						{
							float scale = (float)evt->rampdown/(float)evt->ramp_start;
							s[n] += evt->buffer[t] * scale;
							++t;
							evt->rampdown--;
						}
					}

					// Add internal buffer counter to "global" event counter.
					evt->t += evt->buffer_size;

					if((evt->t < af->size) && (evt->rampdown != 0))
					{
						evt->buffer = audioCache.next(evt->cache_id, evt->buffer_size);
					}
					else
					{
						removeevent = true;
					}

					if(removeevent)
					{
						audioCache.close(evt->cache_id);
					}
				}
			}
			break;
		}

		if(removeevent)
		{
			delete event;
			i = activeevents[ch].erase(i);
			continue;
		}
	}
}

void DrumGizmo::stop()
{
	// engine.stop();
}

int DrumGizmo::samplerate()
{
	return Conf::samplerate;
}

void DrumGizmo::setSamplerate(int samplerate)
{
	DEBUG(dgeditor, "%s samplerate: %d\n", __PRETTY_FUNCTION__, samplerate);
	Conf::samplerate = samplerate;
#ifdef WITH_RESAMPLER
	for(int i = 0; i < MAX_NUM_CHANNELS; ++i)
	{
		resampler[i].setup(kit.samplerate(), Conf::samplerate);
	}
	if(resampler[0].ratio() != 1)
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
	if(ie->isMidiEngine())
	{
		AudioInputEngineMidi *aim = (AudioInputEngineMidi*)ie;
		mmapfile = aim->midimapFile();
	}

	return
		"<config>\n"
		"  <value name=\"drumkitfile\">" + kit.file() + "</value>\n"
		"  <value name=\"midimapfile\">" + mmapfile + "</value>\n"
		"  <value name=\"enable_velocity_modifier\">" +
		bool2str(Conf::enable_velocity_modifier) + "</value>\n"
		"  <value name=\"velocity_modifier_falloff\">" +
		float2str(Conf::velocity_modifier_falloff) + "</value>\n"
		"  <value name=\"velocity_modifier_weight\">" +
		float2str(Conf::velocity_modifier_weight) + "</value>\n"
		"  <value name=\"enable_velocity_randomiser\">" +
		bool2str(Conf::enable_velocity_randomiser) + "</value>\n"
		"  <value name=\"velocity_randomiser_weight\">" +
		float2str(Conf::velocity_randomiser_weight) + "</value>\n"
		"</config>";
}

bool DrumGizmo::setConfigString(std::string cfg)
{
	DEBUG(config, "Load config: %s\n", cfg.c_str());

	std::string dkf;
	ConfigParser p;
	if(p.parse(cfg))
	{
	 ERR(drumgizmo, "Config parse error.\n");
	 return false;
	}

	if(p.value("enable_velocity_modifier") != "")
	{
		Conf::enable_velocity_modifier =
			p.value("enable_velocity_modifier") == "true";
	}

	if(p.value("velocity_modifier_falloff") != "")
	{
		Conf::velocity_modifier_falloff =
			str2float(p.value("velocity_modifier_falloff"));
	}

	if(p.value("velocity_modifier_weight") != "")
	{
		Conf::velocity_modifier_weight =
			str2float(p.value("velocity_modifier_weight"));
	}

	if(p.value("enable_velocity_randomiser") != "")
	{
		Conf::enable_velocity_randomiser =
			p.value("enable_velocity_randomiser") == "true";
	}

	if(p.value("velocity_randomiser_weight") != "")
	{
		Conf::velocity_randomiser_weight =
			str2float(p.value("velocity_randomiser_weight"));
	}

	if(p.value("enable_resampling") != "")
	{
		Conf::enable_resampling =
			p.value("enable_resampling") == "true";
	}

	std::string newkit = p.value("drumkitfile");
	if(newkit != "" && kit.file() != newkit)
	{
		/*
		  if(!loadkit(p.values["drumkitfile"]))
		  {
		  return false;
		  }
		  init(true);
		*/
		LoadDrumKitMessage *msg = new LoadDrumKitMessage();
		msg->drumkitfile = newkit;
		msghandler.sendMessage(MSGRCV_ENGINE, msg);
	}

	std::string newmidimap = p.value("midimapfile");
	if(newmidimap != "")
	{
		//midimapfile = newmidimap;
		LoadMidimapMessage *msg = new LoadMidimapMessage();
		msg->midimapfile = newmidimap;
		msghandler.sendMessage(MSGRCV_ENGINE, msg);
	}

	return true;
}

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            jackmidi.cc
 *
 *  Mo 25. Jan 11:26:06 CET 2016
 *  Copyright 2016 Christian Glöckner
 *  cgloeckner@freenet.de
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
#include <iostream>
#include <cassert>

#include "cpp11fix.h" // required for c++11
#include "jackmidi.h"

int const NOTE_ON = 0x90;

JackMidiInputEngine::JackMidiInputEngine(JackClient &client)
    : AudioInputEngine{}, JackProcess{}, client(client), port{nullptr},
      midimap{}, midi_mapper{}, pos{0u}, list{nullptr}, listsize{0u}
{
	client.add(*this);
}

JackMidiInputEngine::~JackMidiInputEngine() { client.remove(*this); }

bool JackMidiInputEngine::isMidiEngine() { return true; }

bool JackMidiInputEngine::init(Instruments &instruments)
{
	if (midimap == "")
	{
		std::cerr << "[JackMidiInputEngine] Missing midimap filename\n";
		return false;
	}
	MidiMapParser p{midimap};
	if (p.parse())
	{
		std::cerr << "[JackmidiInputEngine] Failed to parse midimap '" << midimap
		          << "'\n";
		return false;
	}
	midi_mapper.midimap = p.midimap;
	for (auto i = 0u; i < instruments.size(); ++i)
	{
		auto name = instruments[i]->name();
		midi_mapper.instrmap[name] = i;
	}
	port = std::make_unique<JackPort>(client, "drumgizmo_midiin",
	                                  JACK_DEFAULT_MIDI_TYPE, JackPortIsInput);
	return true;
}

void JackMidiInputEngine::setParm(std::string parm, std::string value)
{
	if (parm == "midimap")
	{
		// apply midimap filename
		midimap = value;
	}
	else
	{
		std::cerr << "[JackMidiInputEngine] Unsupported parameter '" << parm
		          << "'\n";
	}
}

bool JackMidiInputEngine::start()
{
	client.activate();
	return true;
}

void JackMidiInputEngine::stop() {}

void JackMidiInputEngine::pre() {}

event_t *JackMidiInputEngine::run(size_t pos, size_t len, size_t *nevents)
{
	*nevents = listsize;
	event_t *l = list;
	printf("Owning raw pointer at drumgizmo/input/jackmidi.cc - GET RID OF "
	       "THEM!\n");
	list = (event_t *)malloc(sizeof(event_t) * 1000);
	listsize = 0;
	return l;
}

void JackMidiInputEngine::post() {}

void JackMidiInputEngine::process(jack_nframes_t num_frames)
{
	assert(port != nullptr);
	void *buffer = jack_port_get_buffer(port->port, num_frames);
	jack_nframes_t num_events = jack_midi_get_event_count(buffer);

	for (jack_nframes_t i = 0; i < num_events; ++i)
	{
		jack_midi_event_t event;
		jack_midi_event_get(&event, buffer, i);
		if (event.size != 3)
		{
			continue;
		}
		if ((event.buffer[0] & NOTE_ON) != NOTE_ON)
		{
			continue;
		}
		int key = event.buffer[1];
		int velocity = event.buffer[2];
		printf("Event key:%d vel:%d\n", key, velocity);
		int k = midi_mapper.lookup(key);
		if (k != -1 && velocity)
		{
			list[listsize].type = TYPE_ONSET;
			list[listsize].instrument = k;
			list[listsize].velocity = velocity / 127.0;
			list[listsize].offset = event.time;
			++listsize;
		}
	}
	jack_midi_clear_buffer(buffer);
	pos += num_frames;
}

/*
    DrumKit* kit;
    size_t pos;
    EventQueue *eventqueue;
*/

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            jackmidi.cc
 *
 *  Mo 25. Jan 11:26:06 CET 2016
 *  Copyright 2016 Christian Gl�ckner
 *  cgloeckner@freenet.de
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
#include <iostream>
#include <cassert>

#include "cpp11fix.h" // required for c++11
#include "jackmidi.h"

JackMidiInputEngine::JackMidiInputEngine(JackClient& client)
	: AudioInputEngineMidi{}
	, JackProcess{}
	, client(client)
	, port{nullptr}
	, pos{0u}
	, events{}
{
	client.add(*this);
}

JackMidiInputEngine::~JackMidiInputEngine()
{
	client.remove(*this);
}

bool JackMidiInputEngine::init(const Instruments& instruments)
{
	if(!loadMidiMap(midimap_file, instruments))
	{
		std::cerr << "[MidifileInputEngine] Failed to parse midimap '"
		          << midimap_file << "'\n";
		return false;
	}
	port = std::make_unique<JackPort>(client, "drumgizmo_midiin",
	                                  JACK_DEFAULT_MIDI_TYPE,
	                                  JackPortIsInput);
	return true;
}

void JackMidiInputEngine::setParm(const std::string& parm, const std::string& value)
{
	if(parm == "midimap")
	{
		// apply midimap filename
		midimap_file = value;
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

void JackMidiInputEngine::stop()
{
}

void JackMidiInputEngine::pre()
{
}

void JackMidiInputEngine::run(size_t pos, size_t len,
                              std::vector<event_t>& events)
{
	assert(events.empty());
	std::swap(events, this->events);
}

void JackMidiInputEngine::post()
{
}

bool JackMidiInputEngine::isFreewheeling() const
{
	return client.isFreewheeling();
}

void JackMidiInputEngine::process(jack_nframes_t num_frames)
{
	assert(port != nullptr);
	void* buffer = jack_port_get_buffer(port->port, num_frames);
	jack_nframes_t num_events = jack_midi_get_event_count(buffer);

	// The event list is assumed to be empty here.
	// It might not be though in case the system is under heavy load.
	// Make room for both the new and old events to make sure we don't throw
	// anything away.
	events.reserve(events.size() + num_events);

	for(jack_nframes_t i = 0; i < num_events; ++i)
	{
		jack_midi_event_t event;
		jack_midi_event_get(&event, buffer, i);
		processNote(event.buffer, event.size, event.time, events);
	}

	pos += num_frames;
}

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            midiinputengine.cc
 *
 *  Sat May 14 13:26:23 CEST 2016
 *  Copyright 2016 Bent Bisballe Nyeng
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
#include "midiinputengine.h"

#include <iostream>

static int const NOTE_ON = 0x90;
static int const NOTE_MASK = 0xF0;

MidifileInputEngine::MidifileInputEngine()
	: AudioInputEngineMidi{}
	, smf{nullptr}
	, current_event{nullptr}
	, file{}
	, speed{1.f}
	, track{-1} // all tracks
	, loop{false}
	, offset{0.0}
	, samplerate{44100.0} // todo: via ctor arg
{
}

MidifileInputEngine::~MidifileInputEngine()
{
	if(smf != nullptr)
	{
		smf_delete(smf);
	}
}

bool MidifileInputEngine::init(const Instruments& instruments)
{
	if(file == "")
	{
		std::cerr << "[MidifileInputEngine] Missing midi filename\n";
		return false;
	}
	//if(midimap_filename == "")
	//{
	//	std::cerr << "[MidifileInputEngine] Missing midimap filename\n";
	//	return false;
	//}
	smf = smf_load(file.c_str());
	if(smf == nullptr)
	{
		std::cerr << "[MidifileInputEngine] Failed to load midifile '" << file
		          << "'\n";
		return false;
	}
	//if(!loadMidiMap(midimap_filename, instruments))
	//{
	//	std::cerr << "[MidifileInputEngine] Failed to parse midimap '"
	//	          << midimap_filename << "'\n";
	//	return false;
	//}

	return true;
}

void MidifileInputEngine::setParm(const std::string& parm, const std::string& value)
{
	if(parm == "file")
	{
		// apply midi input filename
		file = value;
	}
	else if(parm == "speed")
	{
		// try to apply speed
		try
		{
			speed = std::stof(value);
		}
		catch(...)
		{
			std::cerr << "[MidifileInputEngine] Invalid speed " << value
			          << "\n";
		}
	}
	//else if(parm == "midimap")
	//{
	//	// apply midimap filename
	//	midimap_filename = value;
	//}
	else if(parm == "loop")
	{
		// apply looping
		loop = true;
	}
	else
	{
		std::cerr << "[MidifileInputEngine] Unsupported parameter '" << parm
		          << "'\n";
	}
}

bool MidifileInputEngine::start()
{
	return true;
}

void MidifileInputEngine::stop()
{
}

void MidifileInputEngine::pre()
{
}

void MidifileInputEngine::run(size_t pos, size_t len, std::vector<event_t>& events)
{
	assert(events.empty());

	double current_max_time = (1.0 + pos + len) / (samplerate / speed);
	current_max_time -= offset;

	if(!current_event)
	{
		current_event = smf_get_next_event(smf);
	}

	while(current_event && current_event->time_seconds < current_max_time)
	{
		if(!smf_event_is_metadata(current_event))
		{
			if((current_event->midi_buffer_length == 3) &&
			    ((current_event->midi_buffer[0] & NOTE_MASK) == NOTE_ON) &&
			    (track == -1 || current_event->track_number == track) &&
			    current_event->midi_buffer[2] > 0)
			{
				int key = current_event->midi_buffer[1];
				int velocity = current_event->midi_buffer[2];

				events.emplace_back();
				auto& event = events.back();
				event.type = TYPE_ONSET;
				size_t evpos = current_event->time_seconds * (samplerate / speed);
				event.offset = evpos - pos;

				int i = mmap.lookup(key);
				if(i != -1)
				{
					event.instrument = i;
					event.velocity = velocity / 127.0;
				}
			}
		}

		current_event = smf_get_next_event(smf);
	}

	if(!current_event)
	{
		if(loop)
		{
			smf_rewind(smf);
			offset += current_max_time;
		}
		else
		{
			assert(len >= 1);
			events.push_back({TYPE_STOP, 0, len-1, 0.f});
		}
	}
}

void MidifileInputEngine::post()
{
}

bool MidifileInputEngine::isFreewheeling() const
{
	return true;
}

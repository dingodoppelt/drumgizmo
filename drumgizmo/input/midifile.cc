/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            midifile.cc
 *
 *  Mi 20. Jan 16:07:57 CET 2016
 *  Copyright 2016 Christian Glöckner
 *  cgloeckner@freenet.de
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
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

#include "midifile.h"

static int const NOTE_ON = 0x90;

MidifileInputEngine::MidifileInputEngine()
	: AudioInputEngineMidi{}
	, smf{nullptr}
	, current_event{nullptr}
	, file{}
	, speed{1.f}
	, track{-1} // all tracks
	, loop{false}
	, offset{0.0}
{
}

MidifileInputEngine::~MidifileInputEngine()
{
	if(smf != nullptr)
	{
		smf_delete(smf);
	}
}

bool MidifileInputEngine::init(Instruments& instruments)
{
	if(file == "")
	{
		std::cerr << "[MidifileInputEngine] Missing midi filename\n";
		return false;
	}
	if(midimap == "")
	{
		std::cerr << "[MidifileInputEngine] Missing midimap filename\n";
		return false;
	}
	smf = smf_load(file.c_str());
	if(smf == nullptr)
	{
		std::cerr << "[MidifileInputEngine] Failed to load midifile '" << file
		          << "'\n";
		return false;
	}
	if(!loadMidiMap(midimap, instruments))
	{
		std::cerr << "[MidifileInputEngine] Failed to parse midimap '"
		          << midimap << "'\n";
		return false;
	}
	return true;
}

void MidifileInputEngine::setParm(std::string parm, std::string value)
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
	else if(parm == "midimap")
	{
		// apply midimap filename
		midimap = value;
	}
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

event_t* MidifileInputEngine::run(size_t pos, size_t len, size_t* nevents)
{
	event_t* evs{nullptr};
	size_t num_events{0u};

	double current_max_time = (1.0 + pos + len) / (44100.0 / speed);
	current_max_time -= offset;
	//  double cur_min_time = (double)(pos) / (44100.0 / speed);

	if(!current_event)
	{
		current_event = smf_get_next_event(smf);
	}

	while(current_event && current_event->time_seconds < current_max_time)
	{
		if(!smf_event_is_metadata(current_event))
		{
			if((current_event->midi_buffer_length == 3) &&
			    ((current_event->midi_buffer[0] & NOTE_ON) == NOTE_ON) &&
			    (track == -1 || current_event->track_number == track) &&
			    current_event->midi_buffer[2] > 0)
			{

				if(evs == nullptr)
				{
					// todo: get rid of malloc
					evs = (event_t*)malloc(sizeof(event_t) * 1000);
				}

				int key = current_event->midi_buffer[1];
				int velocity = current_event->midi_buffer[2];

				evs[num_events].type = TYPE_ONSET;
				size_t evpos = current_event->time_seconds * (44100.0 / speed);
				evs[num_events].offset = evpos - pos;

				int i = mmap.lookup(key);
				if(i != -1)
				{
					evs[num_events].instrument = i;
					evs[num_events].velocity = velocity / 127.0;

					++num_events;
					if(num_events > 999)
					{
						fprintf(stderr, "PANIC!\n");
						break;
					}
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
			if(evs == nullptr)
			{
				// TODO: get rid of malloc
				evs = (event_t*)malloc(sizeof(event_t) * 1000);
			}
			evs[num_events].type = TYPE_STOP;
			evs[num_events].offset = len - 1;
			++num_events;
		}
	}
	*nevents = num_events;
	return evs;
}

void MidifileInputEngine::post()
{
}

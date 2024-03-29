/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            midifile.cc
 *
 *  Mi 20. Jan 16:07:57 CET 2016
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

#include "midifile.h"

MidifileInputEngine::MidifileInputEngine()
	: AudioInputEngineMidi{}
	, smf{nullptr}
	, current_event{nullptr}
	, file{}
	, speed{1.f}
	, track{-1} // all tracks
	, loop{false}
	, offset{0.0}
	, samplerate{44100.0}
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
	if(midimap_file == "")
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
	if(!loadMidiMap(midimap_file, instruments))
	{
		std::cerr << "[MidifileInputEngine] Failed to parse midimap '"
		          << midimap_file << "'\n";
		return false;
	}
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
	else if(parm == "midimap")
	{
		// apply midimap filename
		midimap_file = value;
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
			if(track == -1 || current_event->track_number == track)
			{
				processNote(current_event->midi_buffer,
				            current_event->midi_buffer_length,
				            current_event->time_seconds * (samplerate / speed),
				            events);
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
			events.push_back({EventType::Stop, 0, len-1, 0.f});
		}
	}
}

void MidifileInputEngine::post()
{
}

void MidifileInputEngine::setSampleRate(double sample_rate)
{
	this->samplerate = sample_rate;
}

bool MidifileInputEngine::isFreewheeling() const
{
	return true;
}

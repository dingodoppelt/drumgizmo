/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audioinputenginemidi.cc
 *
 *  Mon Apr  1 20:13:25 CEST 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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
#include "audioinputenginemidi.h"

#include "midimapparser.h"

#include "drumgizmo.h"

#include <hugin.hpp>

AudioInputEngineMidi::AudioInputEngineMidi()
	: refs(REFSFILE)
{
	is_valid = false;
}

bool AudioInputEngineMidi::loadMidiMap(const std::string& file,
                                       const Instruments& instruments)
{
	std::string f = file;

	if(refs.load())
	{
		if(file.size() > 1 && file[0] == '@')
		{
			f = refs.getValue(file.substr(1));
		}
	}
	else
	{
		ERR(drumkitparser, "Error reading refs.conf");
	}

	midimap = "";
	is_valid = false;

	DEBUG(mmap, "loadMidiMap(%s, i.size() == %d)\n", f.c_str(),
	      (int)instruments.size());

	if(f == "")
	{
		return false;
	}

	MidiMapParser midimap_parser;
	if(!midimap_parser.parseFile(f))
	{
		return false;
	}

	instrmap_t instrmap;
	for(size_t i = 0; i < instruments.size(); i++)
	{
		instrmap[instruments[i]->getName()] = i;
	}

	mmap.swap(instrmap, midimap_parser.midimap);

	midimap = file;
	is_valid = true;

	return true;
}

std::string AudioInputEngineMidi::getMidimapFile() const
{
	return midimap;
}

bool AudioInputEngineMidi::isValid() const
{
	return is_valid;
}

// Note types:
static const std::uint8_t NoteOff = 0x80;
static const std::uint8_t NoteOn = 0x90;
static const std::uint8_t NoteAftertouch = 0xA0;

// Note type mask:
static int const NoteMask = 0xF0;

void AudioInputEngineMidi::processNote(const std::uint8_t* midi_buffer,
                                       std::size_t midi_buffer_length,
                                       std::size_t offset,
                                       std::vector<event_t>& events)
{
	if(midi_buffer_length < 3)
	{
		return;
	}

	auto key = midi_buffer[1];
	auto velocity = midi_buffer[2];
	auto instrument_idx = mmap.lookup(key);

	switch(midi_buffer[0] & NoteMask)
	{
	case NoteOff:
		// Ignore for now
		break;

	case NoteOn:
		if(velocity != 0 && instrument_idx != -1)
		{
			// maps velocities to [.5/127, 126.5/127]
			auto centered_velocity = (velocity-.5f)/127.0f;
			events.push_back({EventType::OnSet, (std::size_t)instrument_idx,
			                  offset, centered_velocity});
		}
		break;

	case NoteAftertouch:
		if(velocity == 0 && instrument_idx != -1)
		{
			events.push_back({EventType::Choke, (std::size_t)instrument_idx,
			                  offset, .0f});
		}
		break;

	default:
		break;
	}
}

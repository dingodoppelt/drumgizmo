/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            midimapper.cc
 *
 *  Mon Jul 21 15:24:08 CEST 2008
 *  Copyright 2008 Bent Bisballe Nyeng
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
#include "midimapper.h"

std::vector<int> MidiMapper::lookup(int note_id)
{
	std::vector<int> instruments;

	std::lock_guard<std::mutex> guard(mutex);

	for(const auto& map_entry : midimap)
	{
		if(map_entry.note_id == note_id)
		{
			auto instrmap_it = instrmap.find(map_entry.instrument_name);
			if(instrmap_it != instrmap.end())
			{
				instruments.push_back(instrmap_it->second);
			}
		}
	}

	return instruments;
}

void MidiMapper::swap(instrmap_t& instrmap, midimap_t& midimap)
{
	std::lock_guard<std::mutex> guard(mutex);

	std::swap(this->instrmap, instrmap);
	std::swap(this->midimap, midimap);
}

const midimap_t& MidiMapper::getMap()
{
	return midimap;
}

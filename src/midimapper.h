/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            midimapper.h
 *
 *  Mon Jul 21 15:24:07 CEST 2008
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
#pragma once

#include <map>
#include <string>
#include <mutex>
#include <vector>

struct MidimapEntry
{
	int note_id;
	std::string instrument_name;
};

struct MidiCCmapEntry
{
	int cc_id;
	int note_id;
};

using midimap_t = std::vector<MidimapEntry>;
using instrmap_t = std::map<std::string, int>;
using ccmap_t = std::vector<MidiCCmapEntry>;

class MidiMapper
{
public:
	//! Lookup note in map and returns the corresponding instrument index list.
	std::vector<int> lookup(int note_id);
	std::vector<int> lookupCC(int cc_id);

	//! Set new map sets.
	void swap(instrmap_t& instrmap, midimap_t& midimap, ccmap_t& ccmap);

	const midimap_t& getMap();
	ccmap_t ccmap;

private:
	instrmap_t instrmap;
	midimap_t midimap;

	std::mutex mutex;
};

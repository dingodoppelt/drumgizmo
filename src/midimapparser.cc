/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            midimapparser.cc
 *
 *  Mon Aug  8 16:55:30 CEST 2011
 *  Copyright 2011 Bent Bisballe Nyeng
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
#include "midimapparser.h"

#include <pugixml.hpp>
#include <hugin.hpp>

bool MidiMapParser::parseFile(const std::string& filename)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.data());
	if(result.status)
	{
		ERR(midimapparser, "XML parse error: %d", (int)result.offset);
		return false;
	}

	pugi::xml_node midimap_node = doc.child("midimap");
	for(pugi::xml_node map_node : midimap_node.children("map"))
	{
		constexpr int bad_value = 10000;
		auto note = map_node.attribute("note").as_int(bad_value);
		auto instr = map_node.attribute("instr").as_string();
		auto cc = map_node.attribute("cc").as_int(bad_value);
        auto mode = map_node.attribute("mode").as_string();
		if(note != bad_value)
		{
			if(instr !=  "" && cc ==  bad_value)
			{
				MidimapEntry entry{note, instr};
				midimap.push_back(entry);
				continue;
			}
			else if(cc != bad_value && instr == "")
			{
				if(mode == "") mode = "param";
				MidiCCmapEntry entry{cc, note, mode};
				ccmap.push_back(entry);
				continue;
			}
			else
			{
				continue;
			}
		}
		else
		{
			continue;
		}
	}

	return true;
}

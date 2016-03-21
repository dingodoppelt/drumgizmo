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
#include "midimapparser.h"

MidiMapParser::MidiMapParser(const std::string& file)
{
	fd = fopen(file.c_str(), "r");
}

MidiMapParser::~MidiMapParser()
{
	if(fd)
	{
		fclose(fd);
	}
}

void MidiMapParser::startTag(const std::string& name, attr_t& attr)
{
	if(name == "map")
	{
		if(attr.find("note") != attr.end() && attr.find("instr") != attr.end())
		{
			midimap[std::stoi(attr["note"])] = attr["instr"];
		}
	}
}

int MidiMapParser::readData(std::string& data, std::size_t size)
{
	if(!fd)
	{
		return -1;
	}

	return fread((char*)data.c_str(), 1, size, fd);
}

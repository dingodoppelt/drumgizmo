/* -*- Mode: c++ -*- */
/***************************************************************************
 *            DGDOM.h
 *
 *  Thu Jun  7 11:07:17 CEST 2018
 *  Copyright 2018 Bent Bisballe Nyeng
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

#include <vector>
#include <string>

// Instrument DOM:

struct AudioFileDOM
{
	std::string instrument_channel;
	std::string file;
	std::size_t filechannel;
};

struct SampleDOM
{
	std::string name;
	double power;
	std::vector<AudioFileDOM> audiofiles;
};

struct InstrumentChannelDOM
{
	std::string name;
};

struct InstrumentDOM
{
	std::string name;
	std::vector<SampleDOM> samples;
	std::vector<InstrumentChannelDOM> instrument_channels;
};


// Drumkit DOM:

struct ChannelDOM
{
	std::string name;
};

struct ChannelMapDOM
{
	std::string in;
	std::string out;
	bool main;
};

struct InstrumentRefDOM
{
	std::string name;
	std::string file; // Probably shouldn't be there
	std::string group;
	std::vector<ChannelMapDOM> channel_map;
};

struct DrumkitDOM
{
	std::string name;
	std::string description;
	double samplerate;

	std::vector<InstrumentRefDOM> instruments;
	std::vector<ChannelDOM> channels;
};

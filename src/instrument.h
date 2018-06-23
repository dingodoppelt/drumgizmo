/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            instrument.h
 *
 *  Tue Jul 22 17:14:19 CEST 2008
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

#include <string>
#include <vector>
#include <memory>
#include <deque>

#include "powerlist.h"

#include "sample.h"
#include "versionstr.h"
#include "random.h"

#include "settings.h"

class Instrument
{
public:
	Instrument(Settings& settings, Random& rand);
	~Instrument();

	Sample* sample(level_t level, size_t pos);

	const std::string& getName() const;
	const std::string& getDescription() const;
	const std::string& getGroup() const;

	void setGroup(const std::string& group);

	//  std::map<std::string, std::string> channelmap;

	std::vector<std::unique_ptr<AudioFile>> audiofiles;

	bool isValid() const;

	//! Get the number of audio files (as in single channel) in this instrument.
	std::size_t getNumberOfFiles() const;

	float getMaxPower() const;
	float getMinPower() const;

private:
	friend class DOMLoader;
	friend class DOMLoaderTest;

	// For parser:
	friend class InstrumentParser;
	friend class DrumKitParser;

	// For unit-tests:
	friend class InstrumentParserTest;

	void* magic;

	std::string _group;
	std::string _name;
	std::string _description;

	VersionStr version;

	void addSample(level_t a, level_t b, Sample* s);
	void finalise(); ///< Signal instrument that no more samples will be added.

	std::vector<Sample*> samplelist;
	std::deque<InstrumentChannel> instrument_channels;

	size_t lastpos;
	float mod;
	Settings& settings;
	Random& rand;
	PowerList powerlist;
};

// typedef std::map< std::string, Instrument > Instruments;
using Instruments = std::vector<std::unique_ptr<Instrument>>;

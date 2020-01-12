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

#include "id.h"
#include "powerlist.h"
#include "rangemap.h" // for v1.0 kits
#include "random.h"
#include "sample_selection.h"
#include "sample.h"
#include "versionstr.h"

#include "settings.h"

struct Choke;

class Instrument
{
public:
	Instrument(Settings& settings, Random& rand);
	~Instrument();

	const Sample* sample(level_t level, size_t pos);

	std::size_t getID() const;
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

	const std::vector<Choke>& getChokes();

private:
	// For unit-tests:
	friend class DOMLoaderTest;

	// For parser:
	friend class DOMLoader;

	void* magic;

	std::size_t id;
	std::string _group;
	std::string _name;
	std::string _description;

	VersionStr version;

	RangeMap<level_t, const Sample*> samples;

	void addSample(level_t a, level_t b, const Sample* s);
	void finalise(); ///< Signal instrument that no more samples will be added.

	std::vector<Sample*> samplelist;
	std::deque<InstrumentChannel> instrument_channels;

	size_t lastpos;
	float mod;
	Settings& settings;
	Random& rand;
	PowerList powerlist;
	std::vector<Choke> chokes;
	SampleSelection sample_selection;
};

using Instruments = std::vector<std::unique_ptr<Instrument>>;
using InstrumentID = ID<Instrument>;
using InstrumentIDs = std::vector<InstrumentID>;

struct Choke
{
	std::size_t instrument_id;
	double choketime;
};

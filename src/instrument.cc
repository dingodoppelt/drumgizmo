/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            instrument.cc
 *
 *  Tue Jul 22 17:14:20 CEST 2008
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
#include "instrument.h"

#include <hugin.hpp>

#include "sample.h"

Instrument::Instrument(Settings& settings, Random& rand)
	: settings(settings)
	, rand(rand)
	, powerlist(rand, settings)
{
	DEBUG(instrument, "new %p\n", this);
	mod = 1.0;
	lastpos = 0;

	magic = this;
}

Instrument::~Instrument()
{
	magic = nullptr;

	DEBUG(instrument, "delete %p\n", this);
}

bool Instrument::isValid() const
{
	return this == magic;
}

Sample* Instrument::sample(level_t level, size_t pos)
{
	Sample *sample = nullptr;

	if(version >= VersionStr("2.0"))
	{
		// Version 2.0
		sample = powerlist.get(level * mod);
	}
	else
	{
		// Version 1.0
		std::vector<Sample*> s = samples.get(level * mod);
		if(s.size() == 0)
		{
			return nullptr;
		}

		sample = rand.choose(s);
	}

	return sample;
}

void Instrument::addSample(level_t a, level_t b, Sample* s)
{
	samples.insert(a, b, s);
}

void Instrument::finalise()
{
	if(version >= VersionStr("2.0"))
	{
		std::vector<Sample*>::iterator s = samplelist.begin();
		while(s != samplelist.end())
		{
			powerlist.add(*s);
			s++;
		}

		powerlist.finalise();
	}
}

const std::string& Instrument::getName() const
{
	return _name;
}

const std::string& Instrument::getDescription() const
{
	return _description;
}

const std::string& Instrument::getGroup() const
{
	return _group;
}

void Instrument::setGroup(const std::string& g)
{
	_group = g;
}

std::size_t Instrument::getNumberOfFiles() const
{
	DEBUG(instrument, "audiofiles.size() %d", (int)audiofiles.size());

	// Note: Each AudioFile instance contains just a single channel even for
	// multi-channel files.
	return audiofiles.size();
}

float Instrument::getMaxPower() const
{
	if(version >= VersionStr("2.0"))
	{
		return powerlist.getMaxPower();
	}
	else
	{
		return 1.0f;
	}
}

float Instrument::getMinPower() const
{
	if(version >= VersionStr("2.0"))
	{
		return powerlist.getMinPower();
	}
	else
	{
		return 0.0f;
	}
}

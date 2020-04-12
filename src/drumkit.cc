/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumkit.cc
 *
 *  Wed Mar  9 15:27:27 CET 2011
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
#include "drumkit.h"

DrumKit::DrumKit()
{
	magic = this;
}

DrumKit::~DrumKit()
{
	magic = NULL;
	clear();
}

void DrumKit::clear()
{
	instruments.clear();

	channels.clear();

	// Set all metadata fields to empty values.
	metadata = {};

	// Explicitly set the samplerate default value, since this cannot be 0
	metadata._samplerate = 44100.0f;
}

bool DrumKit::isValid() const
{
	return this == magic;
}

std::string DrumKit::getFile() const
{
	return _file;
}

std::string DrumKit::getName() const
{
	return metadata._name;
}

std::string DrumKit::getDescription() const
{
	return metadata._description;
}

VersionStr DrumKit::getVersion() const
{
	return metadata._version;
}

float DrumKit::getSamplerate() const
{
	return metadata._samplerate;
}

std::size_t DrumKit::getNumberOfFiles() const
{
	std::size_t number_of_files = 0;

	for(const auto & instrument : instruments)
	{
		number_of_files += instrument->getNumberOfFiles();
	}

	return number_of_files;
}

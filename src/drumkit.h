/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumkit.h
 *
 *  Wed Mar  9 15:27:26 CET 2011
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
#pragma once

#include <map>
#include <string>

#include "channel.h"
#include "instrument.h"
#include "versionstr.h"

class DrumKit
{
	friend class DrumKitParser;
	friend class DrumKitLoader;
public:
	DrumKit();
	~DrumKit();

	std::string getFile() const;

	std::string getName() const;
	std::string getDescription() const;
	VersionStr getVersion() const;

	Instruments instruments;
	Channels channels;

	void clear();

	bool isValid() const;

	float getSamplerate() const;

	//! Get the number of audio files (as in single channel) in this drumkit.
	std::size_t getNumberOfFiles() const;

private:
	friend class DrumkitParserTest;

	void* magic{nullptr};

	std::string _file;

	std::string _name;
	std::string _description;
	float _samplerate{44100.0f};

	VersionStr _version;
};

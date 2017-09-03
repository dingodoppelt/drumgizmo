/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audiofile.h
 *
 *  Tue Jul 22 17:14:11 CEST 2008
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
#include <map>
#include <vector>
#include <limits>
#include <mutex>

#include <sndfile.h>

#include "audio.h"
#include "channel.h"

class InstrumentChannel;

class AudioFile
{
public:
	AudioFile(const std::string& filename, std::size_t filechannel,
	          InstrumentChannel* instrument_channel = nullptr);
	~AudioFile();

	void load(std::size_t sample_limit = std::numeric_limits<std::size_t>::max());
	void unload();

	bool isLoaded() const;

	volatile std::size_t size{0}; // Full size of the file
	volatile std::size_t preloadedsize{0}; // Number of samples preloaded (in data)
	sample_t* data{nullptr};

	std::string filename;

	bool isValid() const;

	//! Returns if this audio file is to be played on a main channel (ie. not a
	//! secondary channel)
	main_state_t mainState() const;

	std::mutex mutex;

	std::size_t filechannel;

private:
	void* magic{nullptr};
	volatile bool is_loaded{false};
	InstrumentChannel* instrument_channel;
};

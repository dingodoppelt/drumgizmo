/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumkit_creator.h
 *
 *  Thu Jan 12 18:51:34 CET 2017
 *  Copyright 2017 André Nusser
 *  andre.nusser@googlemail.com
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
#include <cstdint>

namespace drumkit_creator
{

using Sample = uint16_t;

//! If is_random is true then this overrules the sample member. If is_random
//! is false however, every sample is chosen as the one in the member variable.
struct WavInfo
{
	const std::string filename;
	const std::size_t length;

	const bool is_random;
	const Sample sample;

	WavInfo(const std::string& filename, std::size_t length)
		: filename(filename), length(length), is_random(true), sample(0) {}

	WavInfo(const std::string& filename, std::size_t length, Sample sample)
		: filename(filename), length(length), is_random(false), sample(sample) {}
};

struct Audiofile
{
	WavInfo* wav_info;
	std::size_t filechannel;
};

struct SampleData
{
	std::string name;
	// Vector of non-owning pointers and therefore it is raw.
	std::vector<Audiofile> audiofiles;
};

struct InstrumentData
{
	std::string name;
	std::string filename;
	std::vector<SampleData> sample_data;
};

struct DrumkitData
{
	std::string name;
	std::size_t number_of_channels;
	std::vector<InstrumentData> instruments;
	std::vector<WavInfo> wav_infos;
};

//! Creates a drumkit from data in the temporary directory of the OS and
//! returns its filename.
std::string create(const DrumkitData& data);

//! Creates a single wav file
void createWav(const WavInfo& wav_info, std::size_t number_of_channels, const std::string& dir);

//! Those functions create some special wav files, drumkits, and midimaps
std::string createStdKit(const std::string& name);
std::string createSmallKit(const std::string& name);
std::string createHugeKit(const std::string& name);

std::string createSingleChannelWav(const std::string& name);
std::string createMultiChannelWav(const std::string& name);
std::string create0000Wav(const std::string& name);

std::string createStdMidimap(const std::string& name);

} // end drumkit_creator

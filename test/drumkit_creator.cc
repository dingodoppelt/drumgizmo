/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumkit_creator.cc
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
#include "drumkit_creator.h"

#include <unistd.h>

#include "../src/random.h"

#include <sndfile.h>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>

DrumkitCreator::~DrumkitCreator()
{
	for (const auto& file: created_files)
	{
		auto error = unlink(file.c_str());

		if (error) {
			std::cerr << "File could not be deleted in DrumkitCreator destructor"
			          << std::endl;
		}
	}

	for (const auto& dir: created_directories)
	{
		auto error = rmdir(dir.c_str());

		if (error) {
			std::cerr << "Directory could not be deleted in DrumkitCreator destructor"
			          << std::endl;
		}
	}
}

std::string DrumkitCreator::create(const DrumkitData& data)
{
	std::string drumkit_filename;

	if (is_valid(data))
	{
		auto dir = createTemporaryDirectory("drumkit");

		for (const auto& wav_info: data.wav_infos) {
			createWav(wav_info, data.number_of_channels, dir);
		}

		for (const auto& instrument: data.instruments) {
			createInstrument(instrument, data.number_of_channels, dir);
		}

		drumkit_filename = createDrumkitFile(data, dir);
	}
	else
	{
		throw "DrumkitData not valid";
	}
	
	return drumkit_filename;
}

void DrumkitCreator::createWav(const WavInfo& wav_info, std::size_t number_of_channels, const std::string& dir)
{
	SF_INFO sfinfo;
	sfinfo.samplerate = 44100;
	sfinfo.channels = number_of_channels;
	sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

	std::string filename = dir + "/" + wav_info.filename;
	auto sndfile = sf_open(filename.c_str(), SFM_WRITE, &sfinfo);
	if (!sndfile) {
		throw "The wav file could not be created";
	}

	created_files.push_back(filename);

	auto data_vec = createData(wav_info, number_of_channels);
	sf_write_raw(sndfile, data_vec.data(), 2*data_vec.size());

	sf_write_sync(sndfile);
	sf_close(sndfile);
}

std::string DrumkitCreator::createStdKit(const std::string& name)
{
	std::vector<WavInfo> wav_infos = {
		WavInfo("1011.wav", 1, 0x1110),
		WavInfo("2122.wav", 1, 0x2221)
	};

	std::vector<Audiofile> audiofiles1(4, Audiofile{&wav_infos.front(), 1});
	std::vector<Audiofile> audiofiles2(4, Audiofile{&wav_infos.back(), 1});

	std::vector<SampleData> sample_data1{{"stroke1", std::move(audiofiles1)}};
	std::vector<SampleData> sample_data2{{"stroke1", std::move(audiofiles2)}};

	std::vector<InstrumentData> instruments = {
		InstrumentData{"instr1", "instr1.xml", std::move(sample_data1)},
		InstrumentData{"instr2", "instr2.xml", std::move(sample_data2)}
	};

	auto kit_data = DrumkitData{name, 4, instruments, wav_infos};

	return create(kit_data);
}

std::string DrumkitCreator::createSmallKit(const std::string& name)
{
	std::vector<WavInfo> wav_infos = {
		WavInfo("small_instr.wav", 549833)
	};

	std::vector<Audiofile> audiofiles;
	for (std::size_t i = 0; i < 13; ++i) {
		audiofiles.push_back({&wav_infos.front(), i+1});
	}

	std::vector<SampleData> sample_data{{"stroke1", std::move(audiofiles)}};

	std::vector<InstrumentData> instruments = {
		InstrumentData{"small_instr", "small_instr.xml", std::move(sample_data)}
	};

	auto kit_data = DrumkitData{name, 13, instruments, wav_infos};

	return create(kit_data);
}

std::string DrumkitCreator::createHugeKit(const std::string& name)
{
	std::vector<WavInfo> wav_infos = {
		WavInfo("huge_instr.wav", 549833)
	};

	std::vector<Audiofile> audiofiles;
	for (std::size_t i = 0; i < 13; ++i) {
		audiofiles.push_back({&wav_infos.front(), i+1});
	}

	std::vector<SampleData> sample_data;
	for (std::size_t i = 0; i < 50; ++i) {
		sample_data.push_back({"stroke" + std::to_string(i), audiofiles});
	}

	std::vector<InstrumentData> instruments;
	for (std::size_t i = 0; i < 50; ++i) {
		instruments.push_back({"huge_instr" + std::to_string(i),
							   "huge_instr.xml",
							   sample_data});
	}

	auto kit_data = DrumkitData{name, 13, instruments, wav_infos};

	return create(kit_data);
}

std::string DrumkitCreator::createSingleChannelWav(const std::string& name)
{
	auto dir = createTemporaryDirectory("wavfiles");

	auto wav_info = WavInfo(name, 173516);
	createWav(wav_info,
	          1,
	          dir);

	return dir + "/" + name;
}

std::string DrumkitCreator::createMultiChannelWav(const std::string& name)
{
	auto dir = createTemporaryDirectory("wavfiles");

	auto wav_info = WavInfo(name, 549833);
	createWav(wav_info,
	          13,
	          dir);

	return dir + "/" + name;
}

std::string DrumkitCreator::create0000Wav(const std::string& name)
{
	auto dir = createTemporaryDirectory("wavfiles");

	auto wav_info = WavInfo(name, 1, 0x0000);
	createWav(wav_info,
	          1,
	          dir);

	return dir + "/" + name;
}

std::string DrumkitCreator::createStdMidimap(const std::string& name)
{
	auto dir = createTemporaryDirectory("midimap");

	std::string content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<midimap>\n"
		"<map note=\"1\" instr=\"instr1\"/>\n"
		"<map note=\"2\" instr=\"instr2\"/>\n"
		"</midimap>\n";

	std::string filename = dir + "/" + name + ".xml";
	std::ofstream file;
	file.open(filename);
	if (file.is_open())
	{
		created_files.push_back(filename);
		file << content;
	}
	else
	{
		throw "File could not be opened";
	}
	file.close();

	return filename;
}

//
// private member functions
//

bool DrumkitCreator::is_valid(const DrumkitData& data)
{
	// TODO Check the consistency of the data.
	return true;
}

std::string DrumkitCreator::createTemporaryDirectory(const std::string& name)
{
	std::string dir_template = "/tmp/drumgizmo_" + name + "XXXXXX";
	const auto dir_name = mkdtemp(&dir_template[0]);

	if (dir_name) {
		created_directories.push_back(dir_name);
		return std::string(dir_name);
	}

	return "";
}

auto DrumkitCreator::createData(const WavInfo& wav_info, std::size_t number_of_channels) -> std::vector<Sample>
{
	std::vector<Sample> data_vec(number_of_channels * wav_info.length, wav_info.sample);
	if (wav_info.is_random)
	{
		Random rand(42); // Fix the seed to make it reproducable.
		int lower_bound = std::numeric_limits<Sample>::min();
		int upper_bound = std::numeric_limits<Sample>::max();

		std::generate(data_vec.begin(),
		              data_vec.end(),
		              [&](){ return Sample(rand.intInRange(lower_bound, upper_bound)); });
	}

	return data_vec;
}

void DrumkitCreator::createInstrument(const InstrumentData& data, std::size_t number_of_channels,
                      const std::string& dir)
{
	std::string prefix = "<?xml version='1.0' encoding='UTF-8'?>\n"
		"<instrument name=\"" + data.name + "\">\n";
	// FIXME sampleref
	std::string postfix = "<velocities>\n"
		"<velocity lower=\"0\" upper=\"1\">\n"
		"<sampleref probability=\"1\" name=\"stroke1\"/>\n"
		"</velocity>\n"
		"</velocities>\n"
		"</instrument>\n";

	std::string samples;
	for (const auto& sample: data.sample_data) {
		samples += "<sample name=\"" + sample.name + "\">\n";

		for (std::size_t i = 0; i < sample.audiofiles.size(); ++i)
		{
			const auto& audiofile = sample.audiofiles[i];
			samples += "<audiofile channel=\"ch" + std::to_string(i) + "\" file=\""
						+ audiofile.wav_info->filename + "\" filechannel=\""
						+ std::to_string(audiofile.filechannel) + "\"/>\n";
		}
		samples += "</sample>\n";
	}

	// Write to file
	std::string filename = dir + "/" + data.filename;
	std::ofstream file;
	file.open(filename);
	if (file.is_open())
	{
		created_files.push_back(filename);
		file << prefix + samples + postfix;
	}
	else
	{
		throw "File could not be opened";
	}
	file.close();
}

std::string DrumkitCreator::createDrumkitFile(const DrumkitData& data, const std::string& dir)
{
	// Pre- and postfix string
	std::string prefix = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<drumkit name=\"" + data.name + "\" description=\"An drumkit generated"
		" by the drumkit_generator\">\n";
	std::string postfix = "</drumkit>\n";

	// Channel string
	std::string channels;
	channels += "<channels>\n";
	for (std::size_t i = 0; i < data.number_of_channels; ++i)
	{
		channels += "<channel name=\"ch" + std::to_string(i) + "\"/>\n";
	}
	channels += "</channels>\n";
	
	// Instrument string
	std::string instruments;
	instruments += "<instruments>\n";
	for (const auto& instrument: data.instruments)
	{
		instruments += "<instrument name=\"" + instrument.name + "\" file=\"" + instrument.filename + "\">\n";
		for (std::size_t i = 0; i < data.number_of_channels; ++i)
		{
			std::string i_str = std::to_string(i);
			instruments += "<channelmap in=\"ch" + i_str + "\" out=\"ch" + i_str + "\"/>\n";
		}
		instruments += "</instrument>\n";
	}
	instruments += "</instruments>\n";

	// Write everything to a drumkit file
	std::string filename = dir + "/" + data.name + ".xml";
	std::ofstream file;
	file.open(filename);
	if (file.is_open())
	{
		created_files.push_back(filename);
		file << prefix + channels + instruments + postfix;
	}
	else
	{
		throw "File could not be opened";
	}
	file.close();

	return filename;
}

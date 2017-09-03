/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            instrumentparser.cc
 *
 *  Wed Mar  9 13:22:24 CET 2011
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
#include "instrumentparser.h"

#include <string.h>
#include <stdio.h>

#include <hugin.hpp>

#include "cpp11fix.h"
#include "path.h"

#include "nolocale.h"

InstrumentParser::InstrumentParser(Instrument& instrument)
	: instrument(instrument)
{

}

int InstrumentParser::parseFile(const std::string& filename)
{
	path = getPath(filename);

	return SAXParser::parseFile(filename);
}

void InstrumentParser::startTag(const std::string& name, const attr_t& attr)
{
	if(name == "instrument")
	{
		if(attr.find("name") != attr.end())
		{
			instrument._name = attr.at("name");
		}

		if(attr.find("description") != attr.end())
		{
			instrument._description = attr.at("description");
		}

		if(attr.find("version") != attr.end())
		{
			try
			{
				instrument.version = VersionStr(attr.at("version"));
			}
			catch(const char* err)
			{
				ERR(instrparser, "Error parsing version number: %s, using 1.0\n", err);
				instrument.version = VersionStr(1,0,0);
			}
		}
		else
		{
			WARN(instrparser, "Missing version number, assuming 1.0\n");
			instrument.version = VersionStr(1,0,0);
		}
	}

	if(name == "channels")
	{
	}

	if(name == "channel")
	{
		if(attr.find("name") == attr.end())
		{
			ERR(instrparser,"Missing channel required attribute 'name'.\n");
			return;
		}

		InstrumentChannel* channel = addOrGetChannel(attr.at("name"));
		channel->main = main_state_t::is_not_main;
		if(attr.find("main") != attr.end())
		{
			channel->main = (attr.at("main") == "true") ?
				main_state_t::is_main : main_state_t::is_not_main;
		}
	}

	if(name == "samples")
	{
	}

	if(name == "sample")
	{
		if(attr.find("name") == attr.end())
		{
			ERR(instrparser,"Missing required attribute 'name'.\n");
			return;
		}

		float power;
		if(attr.find("power") == attr.end())
		{
			power = -1;
		}
		else
		{
			power = atof_nol(attr.at("power").c_str());
			DEBUG(instrparser, "Instrument power set to %f\n", power);
		}

		// TODO get rid of new or delete it properly
		sample = new Sample(attr.at("name"), power);
	}

	if(name == "audiofile")
	{
		if(sample == nullptr)
		{
			ERR(instrparser,"Missing Sample!\n");
			return;
		}

		if(attr.find("file") == attr.end())
		{
			ERR(instrparser,"Missing required attribute 'file'.\n");
			return;
		}

		if(attr.find("channel") == attr.end())
		{
			ERR(instrparser,"Missing required attribute 'channel'.\n");
			return;
		}

		std::size_t filechannel = 1; // default, override with optional attribute
		if(attr.find("filechannel") != attr.end())
		{
			filechannel = std::stoi(attr.at("filechannel"));
			if(filechannel < 1)
			{
				ERR(instrparser,"Invalid value for attribute 'filechannel'.\n");
				filechannel = 1;
			}
		}

		filechannel = filechannel - 1; // 1-based in file but zero-based internally.

		InstrumentChannel *instrument_channel = addOrGetChannel(attr.at("channel"));

		auto audio_file =
			std::make_unique<AudioFile>(path + "/" + attr.at("file"),
			                            filechannel,
			                            instrument_channel);

		sample->addAudioFile(instrument_channel, audio_file.get());

		// Transfer audio_file ownership to the instrument.
		instrument.audiofiles.push_back(std::move(audio_file));
	}

	if(name == "velocities")
	{
	}

	if(name == "velocity")
	{
		if(attr.find("lower") == attr.end())
		{
			ERR(instrparser,"Missing required attribute 'lower'.\n");
			return;
		}

		if(attr.find("upper") == attr.end())
		{
			ERR(instrparser,"Missing required attribute 'upper'.\n");
			return;
		}

		lower = atof_nol(attr.at("lower").c_str());
		upper = atof_nol(attr.at("upper").c_str());
	}

	if(name == "sampleref")
	{
		if(attr.find("name") == attr.end())
		{
			ERR(instrparser,"Missing required attribute 'name'.\n");
			return;
		}

		Sample* sample_ref = nullptr;
		for(auto& sample : instrument.samplelist)
		{
			if(sample->name == attr.at("name"))
			{
				sample_ref = sample;
				break;
			}
		}

		if(sample_ref == nullptr)
		{
			ERR(instrparser,"Sampleref pointed at non-existing sample.\n");
			return;
		}

		if(instrument.version == VersionStr("1.0"))
		{
			// Old "velocity group" algorithm needs this
			instrument.addSample(lower, upper, sample_ref);
		}
	}
}

void InstrumentParser::endTag(const std::string& name)
{
	if(name == "sample")
	{
		if(sample == nullptr)
		{
			ERR(instrparser,"Missing Sample.\n");
			return;
		}

		instrument.samplelist.push_back(sample);

		sample = nullptr;
	}

	if(name == "instrument") {
		instrument.finalise();
	}
}

InstrumentChannel* InstrumentParser::addOrGetChannel(const std::string& name)
{
	for(auto& channel : instrument.instrument_channels)
	{
		if(channel.name == name)
		{
			return &channel;
		}
	}

	instrument.instrument_channels.emplace_back(name);
	InstrumentChannel& channel = instrument.instrument_channels.back();
	channel.main = main_state_t::unset;

	return &channel;
}

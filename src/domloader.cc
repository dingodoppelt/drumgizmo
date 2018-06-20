/* -*- Mode: c++ -*- */
/***************************************************************************
 *            domloader.cc
 *
 *  Sun Jun 10 17:39:01 CEST 2018
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
#include "domloader.h"

#include <unordered_map>

#include <hugin.hpp>

#include "DGDOM.h"
#include "drumkit.h"
#include "path.h"
#include "channel.h"

#include "cpp11fix.h"

struct channel_attribute_t
{
	std::string cname;
	main_state_t main_state;
};

DOMLoader::DOMLoader(Settings& settings, Random& random)
	: settings(settings)
	, random(random)
{
}

bool DOMLoader::loadDom(const std::string& basepath,
                        const DrumkitDOM& dom,
                        const std::vector<InstrumentDOM>& instrumentdoms,
                        DrumKit& drumkit)
{
	settings.has_bleed_control.store(false);

	drumkit._name = dom.name;
	drumkit._version = dom.version;
	drumkit._description = dom.description;
	drumkit._samplerate = dom.samplerate;

	for(const auto& channel: dom.channels)
	{
		drumkit.channels.emplace_back();
		drumkit.channels.back().name = channel.name;
		drumkit.channels.back().num = drumkit.channels.size() - 1;
	}

	for(const auto& instrumentref : dom.instruments)
	{
		bool found{false};

		std::unordered_map<std::string, channel_attribute_t> channelmap;
		for(const auto& map : instrumentref.channel_map)
		{
			channel_attribute_t cattr{map.out, map.main};
			channelmap[map.in] = cattr;
		}

		for(const auto& instrumentdom : instrumentdoms)
		{
			if(instrumentdom.name != instrumentref.name)
			{
				continue;
			}

			auto instrument = std::make_unique<Instrument>(settings, random);
			instrument->setGroup(instrumentref.group);
			instrument->_name = instrumentdom.name;
			instrument->version = instrumentdom.version;
			instrument->_description = instrumentdom.description;

			auto path = getPath(basepath + "/" + instrumentref.file);
			for(const auto& sampledom : instrumentdom.samples)
			{
				auto sample = new Sample(sampledom.name, sampledom.power);
				for(const auto& audiofiledom : sampledom.audiofiles)
				{
					InstrumentChannel *instrument_channel =
						DOMLoader::addOrGetChannel(*instrument,
						                           audiofiledom.instrument_channel);

					auto audio_file =
						std::make_unique<AudioFile>(path + "/" + audiofiledom.file,
						                            audiofiledom.filechannel - 1,
						                            instrument_channel);

					sample->addAudioFile(instrument_channel,
					                     audio_file.get());

					// Transfer audio_file ownership to the instrument.
					instrument->audiofiles.push_back(std::move(audio_file));
				}
				instrument->samplelist.push_back(sample);
			}

			main_state_t default_main_state = main_state_t::unset;
			for(const auto& channel : channelmap)
			{
				if(channel.second.main_state != main_state_t::unset)
				{
					default_main_state = main_state_t::is_not_main;
				}
			}

			// Assign kit channel numbers to instruments channels and reset
			// main_state attribute as needed.
			for(auto& instrument_channel: instrument->instrument_channels)
			{
				channel_attribute_t cattr{instrument_channel.name, main_state_t::unset};
				if(channelmap.find(instrument_channel.name) != channelmap.end())
				{
					cattr = channelmap[instrument_channel.name];
				}

				if(cattr.main_state == main_state_t::unset)
				{
					cattr.main_state = default_main_state;
				}

				if(cattr.main_state != main_state_t::unset)
				{
					instrument_channel.main = cattr.main_state;
				}

				for(auto cnt = 0u; cnt < drumkit.channels.size(); ++cnt)
				{
					if(drumkit.channels[cnt].name == cattr.cname)
					{
						instrument_channel.num = drumkit.channels[cnt].num;
						instrument_channel.name = drumkit.channels[cnt].name;
						if(instrument_channel.main == main_state_t::is_main)
						{
							settings.has_bleed_control.store(true);
						}
					}
				}

				if(instrument_channel.num == NO_CHANNEL)
				{
					ERR(kitparser, "Missing channel '%s' in instrument '%s'\n",
					    instrument_channel.name.c_str(), instrument->getName().c_str());
				}
			}

			instrument->finalise();

			// Transfer ownership to the DrumKit object.
			drumkit.instruments.emplace_back(std::move(instrument));

			found = true;
		}

		if(!found)
		{
			ERR(domloader, "No instrument with name '%s'", instrumentref.name.data());
			return false;
		}
	}

	return true;
}

InstrumentChannel* DOMLoader::addOrGetChannel(Instrument& instrument,
                                              const std::string& name)
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

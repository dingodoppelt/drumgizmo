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
                        DrumKit& drumkit, LogFunction logger)
{
	settings.has_bleed_control.store(false);

	drumkit._name = dom.metadata.title;
	drumkit._version = dom.version;
	drumkit._description = dom.metadata.description;
	drumkit._samplerate = dom.samplerate;

	for(const auto& channel: dom.channels)
	{
		drumkit.channels.emplace_back();
		drumkit.channels.back().name = channel.name;
		drumkit.channels.back().num = drumkit.channels.size() - 1;
	}

	// Pass 1 - handling everything that is instrument name based and ultimately
	// inserts the instrument into the drumkit instrument list which results in
	// it getting an instrument id (ie. the index in the list).
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
						                            audiofiledom.filechannel - 1, // xml is 1-based
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
					logger(LogLevel::Warning, "Missing channel '" +
					       instrument_channel.name + "' in the '" +
					       instrument->getName() + "' instrument.");
				}
			}

			if(instrument->version == VersionStr(1,0,0))
			{
				// Version 1.0 use velocity groups
				for(auto& velocity : instrumentdom.velocities)
				{
					for(const auto& sampleref : velocity.samplerefs)
					{
						bool found_sample{false};
						for(const auto& sample : instrument->samplelist)
						{
							// TODO: What should be done with the probability??
							if(sample->name == sampleref.name)
							{
								instrument->addSample(velocity.lower, velocity.upper, sample);
								found_sample = true;
								break;
							}
						}
						if(!found_sample)
						{
							ERR(kitparser,
							    "Missing sample '%s' from sampleref in instrument '%s'\n",
							    sampleref.name.data(), instrument->getName().data());
							logger(LogLevel::Warning, "Missing sample '" +
							       sampleref.name + "' in the '" +
							       instrument->getName() + "' instrument.");
							return false;
						}
					}
				}
			}

			instrument->finalise();

			// Transfer ownership to the DrumKit object.
			drumkit.instruments.emplace_back(std::move(instrument));
			drumkit.instruments.back()->id = drumkit.instruments.size() - 1;

			found = true;
		}

		if(!found)
		{
			ERR(domloader, "No instrument with name '%s'", instrumentref.name.data());
			logger(LogLevel::Warning, "No instrument with name '" +
			       instrumentref.name + "'.");
			return false;
		}
	}

	// Pass 2 - handle nodes that require instrument name to instrument id
	// mappings
	for(const auto& instrumentref : dom.instruments)
	{
		std::size_t instr_id{0};
		{
			bool found{false};
			for(std::size_t idx = 0; idx < drumkit.instruments.size(); ++idx)
			{
				if(drumkit.instruments[idx]->getName() == instrumentref.name)
				{
					instr_id = idx;
					found = true;
					break;
				}
			}

			if(!found)
			{
				// Missing instrument - skip
				continue;
			}
		}

		for(const auto& choke : instrumentref.chokes)
		{
			std::size_t choke_instr_id{0};
			bool choke_found{false};
			for(std::size_t idx = 0; idx < drumkit.instruments.size(); ++idx)
			{
				if(drumkit.instruments[idx]->getName() == choke.instrument)
				{
					choke_instr_id = idx;
					choke_found = true;
					break;
				}
			}

			if(!choke_found)
			{
				// Missing choke target instrument - skip
				continue;
			}

			drumkit.instruments[instr_id]->chokes.emplace_back();
			drumkit.instruments[instr_id]->chokes.back().instrument_id = choke_instr_id;
			drumkit.instruments[instr_id]->chokes.back().choketime = choke.choketime;
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

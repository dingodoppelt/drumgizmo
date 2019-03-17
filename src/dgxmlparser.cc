/* -*- Mode: c++ -*- */
/***************************************************************************
 *            dgxmlparser.cc
 *
 *  Fri Jun  8 22:04:31 CEST 2018
 *  Copyright 2018 Jonas Suhr Christensen
 *  jsc@umbraculum.org
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
#include "dgxmlparser.h"

#include <unordered_map>

#include <pugixml.hpp>
#include <hugin.hpp>

#include "nolocale.h"

bool probeDrumkitFile(const std::string& filename)
{
	DrumkitDOM d;
	return parseDrumkitFile(filename, d);
}

bool probeInstrumentFile(const std::string& filename)
{
	InstrumentDOM d;
	return parseInstrumentFile(filename, d);
}

static bool assign(double& dest, const std::string& val)
{
	//TODO: figure out how to handle error value 0.0
	dest = atof_nol(val.c_str());
	return true;
}

static bool assign(std::string& dest, const std::string& val)
{
	dest = val;
	return true;
}

static bool assign(std::size_t& dest, const std::string& val)
{
	int tmp = atoi(val.c_str());
	if(tmp < 0) return false;
	dest = tmp;
	return std::to_string(dest) == val;
}

static bool assign(main_state_t& dest, const std::string& val)
{
	if(val != "true" && val != "false")
	{
		return false;
	}
	dest = (val == "true") ? main_state_t::is_main : main_state_t::is_not_main;
	return true;
}

template<typename T>
static bool attrcpy(T& dest, const pugi::xml_node& src, const std::string& attr, bool opt = false)
{
	const char* val = src.attribute(attr.c_str()).as_string(nullptr);
	if(!val)
	{
		if(!opt)
		{
			ERR(dgxmlparser, "Attribute %s not found in %s, offset %d\n",
			    attr.data(), src.path().data(), (int)src.offset_debug());
		}
		return opt;
	}

	if(!assign(dest, std::string(val)))
	{
		ERR(dgxmlparser, "Attribute %s could not be assigned, offset %d\n",
		    attr.data(), (int)src.offset_debug());
		return false;
	}

	return true;
}

template<typename T>
static bool nodecpy(T& dest, const pugi::xml_node& src, const std::string& node, bool opt = false)
{
	auto val = src.child(node.c_str());
	if(val == pugi::xml_node())
	{
		if(!opt)
		{
			ERR(dgxmlparser, "Node %s not found in %s, offset %d\n",
			    node.data(), src.path().data(), (int)src.offset_debug());
		}
		return opt;
	}

	if(!assign(dest, val.text().as_string()))
	{
		ERR(dgxmlparser, "Attribute %s could not be assigned, offset %d\n",
		    node.data(), (int)src.offset_debug());
		return false;
	}

	return true;
}

bool parseDrumkitFile(const std::string& filename, DrumkitDOM& dom)
{
	bool res = true;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.c_str());
	res &= !result.status;
	if(!res)
	{
		ERR(dgxmlparser, "XML parse error: '%s' %d", filename.data(),
		    (int) result.offset);
		return false;
	}

	pugi::xml_node drumkit = doc.child("drumkit");

	dom.version = "1.0";
	res &= attrcpy(dom.version, drumkit, "version", true);
	dom.samplerate = 44100.0;
	res &= attrcpy(dom.samplerate, drumkit, "samplerate", true);

	// Use the old name and description attributes on the drumkit node as fallback
	res &= attrcpy(dom.metadata.title, drumkit, "name", true);
	res &= attrcpy(dom.metadata.description, drumkit, "description", true);

	pugi::xml_node metadata = drumkit.child("metadata");
	if(metadata != pugi::xml_node())
	{
		auto& meta = dom.metadata;
		res &= nodecpy(meta.version, metadata, "version", true);
		res &= nodecpy(meta.title, metadata, "title", true);
		pugi::xml_node logo = metadata.child("logo");
		if(logo != pugi::xml_node())
		{
			res &= attrcpy(meta.logo, logo, "src", true);
		}
		res &= nodecpy(meta.description, metadata, "description", true);
		res &= nodecpy(meta.license, metadata, "license", true);
		res &= nodecpy(meta.notes, metadata, "notes", true);
		res &= nodecpy(meta.author, metadata, "author", true);
		res &= nodecpy(meta.email, metadata, "email", true);
		res &= nodecpy(meta.website, metadata, "website", true);
		pugi::xml_node image = metadata.child("image");
		if(image != pugi::xml_node())
		{
			res &= attrcpy(meta.image, image, "src", true);
			res &= attrcpy(meta.image_map, image, "map", true);
			for(auto clickmap : image.children("clickmap"))
			{
				meta.clickmaps.emplace_back();
				res &= attrcpy(meta.clickmaps.back().instrument,
				               clickmap, "instrument", true);
				res &= attrcpy(meta.clickmaps.back().colour,
				               clickmap, "colour", true);
			}
		}
	}

	pugi::xml_node channels = doc.child("drumkit").child("channels");
	for(pugi::xml_node channel: channels.children("channel"))
	{
		dom.channels.emplace_back();
		res &= attrcpy(dom.channels.back().name, channel, "name");
	}

	pugi::xml_node instruments = doc.child("drumkit").child("instruments");
	for(pugi::xml_node instrument : instruments.children("instrument"))
	{
		dom.instruments.emplace_back();
		auto& instrument_ref = dom.instruments.back();
		res &= attrcpy(instrument_ref.name, instrument, "name");
		res &= attrcpy(instrument_ref.file, instrument, "file");
		res &= attrcpy(instrument_ref.group, instrument, "group", true);

		for(pugi::xml_node cmap: instrument.children("channelmap"))
		{
			instrument_ref.channel_map.emplace_back();
			auto& channel_map_ref = instrument_ref.channel_map.back();
			res &= attrcpy(channel_map_ref.in, cmap, "in");
			res &= attrcpy(channel_map_ref.out, cmap, "out");
			channel_map_ref.main = main_state_t::unset;
			res &= attrcpy(channel_map_ref.main, cmap, "main", true);
		}

		auto num_chokes = std::distance(instrument.children("chokes").begin(),
		                                instrument.children("chokes").end());
		if(num_chokes > 1)
		{
			// error
			ERR(dgxmlparser, "At most one 'chokes' node allowed pr. instrument.");
			res = false;
		}

		if(num_chokes == 1)
		{
			for(pugi::xml_node choke : instrument.child("chokes").children("choke"))
			{
				instrument_ref.chokes.emplace_back();
				auto& choke_ref = instrument_ref.chokes.back();
				res &= attrcpy(choke_ref.instrument, choke, "instrument");
				choke_ref.choketime = 68; // default to 68 ms
				res &= attrcpy(choke_ref.choketime, choke, "choketime", true);
			}
		}
	}

	return res;
}

bool parseInstrumentFile(const std::string& filename, InstrumentDOM& dom)
{
	bool res = true;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.data());
	res &= !result.status;
	if(!res)
	{
		ERR(dgxmlparser, "XML parse error: '%s'", filename.data());
	}
	//TODO: handle version

	pugi::xml_node instrument = doc.child("instrument");
	res &= attrcpy(dom.name, instrument, "name");
	dom.version = "1.0";
	res &= attrcpy(dom.version, instrument, "version", true);
	res &= attrcpy(dom.description, instrument, "description", true);

	pugi::xml_node channels = instrument.child("channels");
	for(pugi::xml_node channel : channels.children("channel"))
	{
		dom.instrument_channels.emplace_back();
		res &= attrcpy(dom.instrument_channels.back().name, channel, "name");
		dom.instrument_channels.back().main = main_state_t::unset;
		res &= attrcpy(dom.instrument_channels.back().main, channel, "main", true);
	}

	INFO(dgxmlparser, "XML version: %s\n", dom.version.data());

	pugi::xml_node samples = instrument.child("samples");
	for(pugi::xml_node sample: samples.children("sample"))
	{
		dom.samples.emplace_back();
		res &= attrcpy(dom.samples.back().name, sample, "name");

		// Power only part of >= v2.0 instruments.
		if(dom.version == "1.0")
		{
			dom.samples.back().power = 0.0;
		}
		else
		{
			res &= attrcpy(dom.samples.back().power, sample, "power");
		}

		for(pugi::xml_node audiofile: sample.children("audiofile"))
		{
			dom.samples.back().audiofiles.emplace_back();
			res &= attrcpy(dom.samples.back().audiofiles.back().instrument_channel,
			               audiofile, "channel");
			res &= attrcpy(dom.samples.back().audiofiles.back().file,
				               audiofile, "file");
			// Defaults to channel 1 in mono (1-based)
			dom.samples.back().audiofiles.back().filechannel = 1;
			res &= attrcpy(dom.samples.back().audiofiles.back().filechannel,
			               audiofile, "filechannel", true);
		}
	}

	// Velocity groups are only part of v1.0 instruments.
	if(dom.version == "1" || dom.version == "1.0" || dom.version == "1.0.0")
	{
		pugi::xml_node velocities = instrument.child("velocities");
		for(pugi::xml_node velocity: velocities.children("velocity"))
		{
			dom.velocities.emplace_back();

			res &= attrcpy(dom.velocities.back().lower, velocity, "lower");
			res &= attrcpy(dom.velocities.back().upper, velocity, "upper");
			for(auto sampleref : velocity.children("sampleref"))
			{
				dom.velocities.back().samplerefs.emplace_back();
				auto& sref = dom.velocities.back().samplerefs.back();
				res &= attrcpy(sref.probability, sampleref, "probability");
				res &= attrcpy(sref.name, sampleref, "name");
			}
		}
	}

	return res;
}

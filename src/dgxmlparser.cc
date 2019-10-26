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

static int getLineNumberFromOffset(const std::string& filename, ptrdiff_t offset)
{
	FILE* fp = fopen(filename.data(), "rt");
	if(!fp)
	{
		return 0;
	}

	int lineno{1};
	char c = 0;
	while((c = fgetc(fp)) != EOF && offset--)
	{
		lineno += c == '\n' ? 1 : 0;
	}
	fclose(fp);
	return lineno;
}

bool probeDrumkitFile(const std::string& filename, LogFunction logger)
{
	DrumkitDOM d;
	return parseDrumkitFile(filename, d, logger);
}

bool probeInstrumentFile(const std::string& filename, LogFunction logger)
{
	InstrumentDOM d;
	return parseInstrumentFile(filename, d, logger);
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

static bool assign(bool& dest, const std::string& val)
{
	if(val == "true" || val == "false")
	{
		dest = val == "true";
		return true;
	}

	return false;
}

template<typename T>
static bool attrcpy(T& dest, const pugi::xml_node& src, const std::string& attr, LogFunction logger, const std::string& filename, bool opt = false)
{
	const char* val = src.attribute(attr.c_str()).as_string(nullptr);
	if(!val)
	{
		if(!opt)
		{
			ERR(dgxmlparser, "Attribute %s not found in %s, offset %d\n",
			    attr.data(), src.path().data(), (int)src.offset_debug());
			if(logger)
			{
				auto lineno = getLineNumberFromOffset(filename, src.offset_debug());
				logger(LogLevel::Error, "Missing attribute '" + attr +
				       "' at line " + std::to_string(lineno));
			}
		}
		return opt;
	}

	if(!assign(dest, std::string(val)))
	{
		ERR(dgxmlparser, "Attribute %s could not be assigned, offset %d\n",
		    attr.data(), (int)src.offset_debug());
		if(logger)
		{
			auto lineno = getLineNumberFromOffset(filename, src.offset_debug());
			logger(LogLevel::Error, "Attribute '" + attr +
			       "' could not be assigned at line " + std::to_string(lineno));
		}
		return false;
	}

	return true;
}

template<typename T>
static bool nodecpy(T& dest, const pugi::xml_node& src, const std::string& node, LogFunction logger, const std::string& filename, bool opt = false)
{
	auto val = src.child(node.c_str());
	if(val == pugi::xml_node())
	{
		if(!opt)
		{
			ERR(dgxmlparser, "Node %s not found in %s, offset %d\n",
			    node.data(), filename.data(), (int)src.offset_debug());
			if(logger)
			{
				auto lineno = getLineNumberFromOffset(filename, src.offset_debug());
				logger(LogLevel::Error, "Node '" + node +
				       "' not found at line " + std::to_string(lineno));
			}
		}
		return opt;
	}

	if(!assign(dest, val.text().as_string()))
	{
		ERR(dgxmlparser, "Attribute %s could not be assigned, offset %d\n",
		    node.data(), (int)src.offset_debug());
		if(logger)
		{
			auto lineno = getLineNumberFromOffset(filename, src.offset_debug());
			logger(LogLevel::Error, "Node '" + node +
			       "' could not be assigned at line " + std::to_string(lineno));
		}
		return false;
	}

	return true;
}

bool parseDrumkitFile(const std::string& filename, DrumkitDOM& dom, LogFunction logger)
{
	bool res = true;

	if(logger)
	{
		logger(LogLevel::Info, "Loading " + filename);
	}

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.c_str());
	res &= !result.status;
	if(!res)
	{
		ERR(dgxmlparser, "XML parse error: '%s' %d", filename.data(),
		    (int) result.offset);
		if(logger)
		{
			auto lineno = getLineNumberFromOffset(filename, result.offset);
			logger(LogLevel::Error, "XML parse error in '" + filename +
			       "': " + result.description() + " at line " +
			       std::to_string(lineno));
		}
		return false;
	}

	pugi::xml_node drumkit = doc.child("drumkit");

	dom.version = "1.0";
	res &= attrcpy(dom.version, drumkit, "version", logger, filename, true);
	dom.samplerate = 44100.0;
	res &= attrcpy(dom.samplerate, drumkit, "samplerate", logger, filename, true);

	// Use the old name and description attributes on the drumkit node as fallback
	res &= attrcpy(dom.metadata.title, drumkit, "name", logger, filename, true);
	res &= attrcpy(dom.metadata.description, drumkit, "description", logger, filename, true);

	pugi::xml_node metadata = drumkit.child("metadata");
	if(metadata != pugi::xml_node())
	{
		auto& meta = dom.metadata;
		res &= nodecpy(meta.version, metadata, "version", logger, filename, true);
		res &= nodecpy(meta.title, metadata, "title", logger, filename, true);
		pugi::xml_node logo = metadata.child("logo");
		if(logo != pugi::xml_node())
		{
			res &= attrcpy(meta.logo, logo, "src", logger, filename, true);
		}
		res &= nodecpy(meta.description, metadata, "description", logger, filename, true);
		res &= nodecpy(meta.license, metadata, "license", logger, filename, true);
		res &= nodecpy(meta.notes, metadata, "notes", logger, filename, true);
		res &= nodecpy(meta.author, metadata, "author", logger, filename, true);
		res &= nodecpy(meta.email, metadata, "email", logger, filename, true);
		res &= nodecpy(meta.website, metadata, "website", logger, filename, true);
		pugi::xml_node image = metadata.child("image");
		if(image != pugi::xml_node())
		{
			res &= attrcpy(meta.image, image, "src", logger, filename, true);
			res &= attrcpy(meta.image_map, image, "map", logger, filename, true);
			for(auto clickmap : image.children("clickmap"))
			{
				meta.clickmaps.emplace_back();
				res &= attrcpy(meta.clickmaps.back().instrument,
				               clickmap, "instrument", logger, filename, true);
				res &= attrcpy(meta.clickmaps.back().colour,
				               clickmap, "colour", logger, filename, true);
			}
		}
		pugi::xml_node default_midimap = metadata.child("defaultmidimap");
		if(default_midimap != pugi::xml_node())
		{
			res &= attrcpy(meta.default_midimap_file, default_midimap, "src", logger, filename, true);
		}
	}

	pugi::xml_node channels = doc.child("drumkit").child("channels");
	for(pugi::xml_node channel: channels.children("channel"))
	{
		dom.channels.emplace_back();
		res &= attrcpy(dom.channels.back().name, channel, "name", logger, filename);
	}

	pugi::xml_node instruments = doc.child("drumkit").child("instruments");
	for(pugi::xml_node instrument : instruments.children("instrument"))
	{
		dom.instruments.emplace_back();
		auto& instrument_ref = dom.instruments.back();
		res &= attrcpy(instrument_ref.name, instrument, "name", logger, filename);
		res &= attrcpy(instrument_ref.file, instrument, "file", logger, filename);
		res &= attrcpy(instrument_ref.group, instrument, "group", logger, filename, true);

		for(pugi::xml_node cmap: instrument.children("channelmap"))
		{
			instrument_ref.channel_map.emplace_back();
			auto& channel_map_ref = instrument_ref.channel_map.back();
			res &= attrcpy(channel_map_ref.in, cmap, "in", logger, filename);
			res &= attrcpy(channel_map_ref.out, cmap, "out", logger, filename);
			channel_map_ref.main = main_state_t::unset;
			res &= attrcpy(channel_map_ref.main, cmap, "main", logger, filename, true);
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
				res &= attrcpy(choke_ref.instrument, choke, "instrument", logger, filename);
				choke_ref.choketime = 68; // default to 68 ms
				res &= attrcpy(choke_ref.choketime, choke, "choketime", logger, filename, true);
			}
		}
	}

	return res;
}

bool parseInstrumentFile(const std::string& filename, InstrumentDOM& dom, LogFunction logger)
{
	bool res = true;

	if(logger)
	{
		logger(LogLevel::Info, "Loading " + filename);
	}

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.data());
	res &= !result.status;
	if(!res)
	{
		WARN(dgxmlparser, "XML parse error: '%s'", filename.data());
		if(logger)
		{
			auto lineno = getLineNumberFromOffset(filename, result.offset);
			logger(LogLevel::Warning, "XML parse error in '" + filename +
			       "': " + result.description() + " at line " +
			       std::to_string(lineno));
		}
	}
	//TODO: handle version

	pugi::xml_node instrument = doc.child("instrument");
	res &= attrcpy(dom.name, instrument, "name", logger, filename);
	dom.version = "1.0";
	res &= attrcpy(dom.version, instrument, "version", logger, filename, true);
	res &= attrcpy(dom.description, instrument, "description", logger, filename, true);

	pugi::xml_node channels = instrument.child("channels");
	for(pugi::xml_node channel : channels.children("channel"))
	{
		dom.instrument_channels.emplace_back();
		res &= attrcpy(dom.instrument_channels.back().name, channel, "name", logger, filename);
		dom.instrument_channels.back().main = main_state_t::unset;
		res &= attrcpy(dom.instrument_channels.back().main, channel, "main", logger, filename, true);
	}

	INFO(dgxmlparser, "XML version: %s\n", dom.version.data());

	pugi::xml_node samples = instrument.child("samples");
	for(pugi::xml_node sample: samples.children("sample"))
	{
		dom.samples.emplace_back();
		res &= attrcpy(dom.samples.back().name, sample, "name", logger, filename);

		// Power only part of >= v2.0 instruments.
		if(dom.version == "1.0")
		{
			dom.samples.back().power = 0.0;
		}
		else
		{
			res &= attrcpy(dom.samples.back().power, sample, "power", logger, filename);
			dom.samples.back().normalized = false;
			res &= attrcpy(dom.samples.back().normalized, sample, "normalized", logger, filename, true);
		}

		for(pugi::xml_node audiofile: sample.children("audiofile"))
		{
			dom.samples.back().audiofiles.emplace_back();
			res &= attrcpy(dom.samples.back().audiofiles.back().instrument_channel,
			               audiofile, "channel", logger, filename);
			res &= attrcpy(dom.samples.back().audiofiles.back().file,
				               audiofile, "file", logger, filename);
			// Defaults to channel 1 in mono (1-based)
			dom.samples.back().audiofiles.back().filechannel = 1;
			res &= attrcpy(dom.samples.back().audiofiles.back().filechannel,
			               audiofile, "filechannel", logger, filename, true);
		}
	}

	// Velocity groups are only part of v1.0 instruments.
	if(dom.version == "1" || dom.version == "1.0" || dom.version == "1.0.0")
	{
		pugi::xml_node velocities = instrument.child("velocities");
		for(pugi::xml_node velocity: velocities.children("velocity"))
		{
			dom.velocities.emplace_back();

			res &= attrcpy(dom.velocities.back().lower, velocity, "lower", logger, filename);
			res &= attrcpy(dom.velocities.back().upper, velocity, "upper", logger, filename);
			for(auto sampleref : velocity.children("sampleref"))
			{
				dom.velocities.back().samplerefs.emplace_back();
				auto& sref = dom.velocities.back().samplerefs.back();
				res &= attrcpy(sref.probability, sampleref, "probability", logger, filename);
				res &= attrcpy(sref.name, sampleref, "name", logger, filename);
			}
		}
	}

	return res;
}

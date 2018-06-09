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

static bool assign(size_t& dest, const std::string& val)
{
	int tmp = atoi(val.c_str());
	if(tmp < 0) return false;
	dest = tmp;
	return std::to_string(dest) == val; 
}

template<typename T>
static bool attrcpy(T& dest, const pugi::xml_node& src, const std::string& attr)
{
	const char* val = src.attribute(attr.c_str()).as_string(nullptr); 
	if(!val) {
		ERR("Attribute %s not found in %s, offset %s\n", attr.c_str(), src.path().c_str(), (int) src.offset_debug());
		return false;
	}

	if(!assign(dest, std::string(val))) {
		ERR("Attribute %s could not be assigned, offset %s\n", attr.c_str(), (int) src.offset_debug());
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

	if(!res) {
		printf("PugiXml error %d\n", (int) result.offset);
		return false;
	}

	//TODO: handle xml version 
	
	pugi::xml_node drumkit = doc.child("drumkit");
	res &= attrcpy(dom.description, drumkit, "description");
	res &= attrcpy(dom.name, drumkit, "name");

	pugi::xml_node channels = doc.child("drumkit").child("channels");
	for(pugi::xml_node channel: channels.children("channel"))
	{
		dom.channels.emplace_back();
		res &= attrcpy(dom.channels.back().name, channel, "name");
	}

	pugi::xml_node instruments = doc.child("drumkit").child("instruments");
	for(pugi::xml_node instrument: instruments.children("instrument"))
	{
		dom.instruments.emplace_back();

		res &= attrcpy(dom.instruments.back().name, instrument, "name");
		res &= attrcpy(dom.instruments.back().file, instrument, "file");
		res &= attrcpy(dom.instruments.back().group, instrument, "group");

		for(pugi::xml_node cmap: instrument.children("channelmap"))
		{
			dom.instruments.back().channel_map.emplace_back();
			res &= attrcpy(dom.instruments.back().channel_map.back().in, cmap, "in");
			res &= attrcpy(dom.instruments.back().channel_map.back().out, cmap, "out");
			//TODO:handle main
		}
	}

	return res;
}

bool parseInstrumentFile(const std::string& filename, InstrumentDOM& dom)
{
	bool res = true;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.c_str());

	res &= !result.status;
	//TODO: handle version

	pugi::xml_node instrument = doc.child("instrument");
	res &= attrcpy(dom.name, instrument, "name");

	pugi::xml_node samples = doc.child("instrument").child("samples");
	for(pugi::xml_node sample: samples.children("sample"))
	{
		dom.samples.emplace_back();
		res &= attrcpy(dom.samples.back().name, sample, "name");
		res &= attrcpy(dom.samples.back().power, sample, "power");

		for(pugi::xml_node audiofile: sample.children("audiofile"))
		{
			dom.samples.back().audiofiles.emplace_back();
			res &= attrcpy(dom.samples.back().audiofiles.back().instrument_channel, audiofile, "channel");
			res &= attrcpy(dom.samples.back().audiofiles.back().file, audiofile, "file");
			res &= attrcpy(dom.samples.back().audiofiles.back().filechannel, audiofile, "filechannel");
		}
	}

	return res;
}

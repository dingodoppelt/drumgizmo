/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            configparser.cc
 *
 *  Sat Jun 29 21:55:02 CEST 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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
#include "configparser.h"

#include <hugin.hpp>
#include <pugixml.hpp>

static bool assign(std::string& dest, const std::string& val)
{
	dest = val;
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
			ERR(configparser, "Attribute %s not found in %s, offset %d\n",
			    attr.data(), src.path().data(), (int)src.offset_debug());
		}
		return opt;
	}

	if(!assign(dest, std::string(val)))
	{
		ERR(configparser, "Attribute %s could not be assigned, offset %d\n",
		    attr.data(), (int)src.offset_debug());
		return false;
	}

	return true;
}

bool ConfigParser::parseString(const std::string& xml)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_buffer(xml.data(), xml.size());
	if(result.status)
	{
		ERR(configparser, "XML parse error: %d", (int)result.offset);
		return false;
	}

	pugi::xml_node config_node = doc.child("config");

	// Config xml without the version tag defaults to 1.0
	std::string version = "1.0";
	attrcpy(version, config_node, "version", true);
	if(version != "1.0")
	{
		ERR(configparser, "Only config v1.0 XML supported.");
		return false;
	}

	for(pugi::xml_node value_node : config_node.children("value"))
	{
		auto name = value_node.attribute("name").as_string();
		if(std::string(name) == "")
		{
			continue;
		}
		auto value = value_node.child_value();
		values[name] = value;
	}

	return true;
}

std::string ConfigParser::value(const std::string& name, const std::string& def)
{
	if(values.find(name) == values.end())
	{
		return def;
	}

	return values[name];
}

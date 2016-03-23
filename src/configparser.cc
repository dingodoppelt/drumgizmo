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

#include "saxparser.h"

ConfigParser::ConfigParser()
{
	str = nullptr;
}

void ConfigParser::characterData(const std::string& data)
{
	if(str)
	{
		str->append(data);
	}
}

void ConfigParser::startTag(const std::string& name, const attr_t& attr)
{
	if(name == "value" && attr.find("name") != attr.end())
	{
		values[attr.at("name")] = "";
		str = &values[attr.at("name")];
	}
}

void ConfigParser::endTag(const std::string& name)
{
	if(name == "value")
	{
		str = nullptr;
	}
}

std::string ConfigParser::value(const std::string& name, const std::string& def)
{
	if(values.find(name) == values.end())
	{
		return def;
	}

	return values[name];
}

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumkitparser.h
 *
 *  Tue Jul 22 16:24:58 CEST 2008
 *  Copyright 2008 Bent Bisballe Nyeng
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
#pragma once

#include <string>

#include "saxparser.h"
#include "drumkit.h"
#include "configfile.h"

class DrumKitParser
	: public SAXParser
{
public:
	DrumKitParser(Settings& setting, DrumKit& kit, Random& rand);
	virtual ~DrumKitParser() = default;

	virtual int parseFile(const std::string& filename) override;

protected:
	void startTag(const std::string& name, const attr_t& attributes) override;
	void endTag(const std::string& name) override;

private:
	DrumKit& kit;
	std::string path;

	struct channel_attribute_t
	{
		std::string cname;
		main_state_t main_state;
	};
	std::unordered_map<std::string, channel_attribute_t> channelmap;
	std::string instr_file;
	std::string instr_name;
	std::string instr_group;

	ConfigFile refs;
	Settings& settings;
	Random& rand;
};

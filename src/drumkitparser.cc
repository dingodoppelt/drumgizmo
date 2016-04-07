/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumkitparser.cc
 *
 *  Tue Jul 22 16:24:59 CEST 2008
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
#include "drumkitparser.h"

#include <string.h>
#include <stdio.h>
#include <hugin.hpp>

#include "instrumentparser.h"
#include "path.h"
#include "drumgizmo.h"

DrumKitParser::DrumKitParser(Settings& settings, DrumKit& k)
	: kit(k)
	, refs(REFSFILE)
	, settings(settings)
{
}

int DrumKitParser::parseFile(const std::string& filename)
{
	auto edited_filename(filename);

	if(refs.load())
	{
		if((filename.size() > 1) && (filename[0] == '@'))
		{
			edited_filename = refs.getValue(filename.substr(1));
		}
	}
	else
	{
		ERR(drumkitparser, "Error reading refs.conf");
	}

	path = getPath(edited_filename);
	auto result = SAXParser::parseFile(edited_filename);

	if(result == 0)
	{
		kit._file = edited_filename;
	}

	return result;
}

void DrumKitParser::startTag(const std::string& name, const attr_t& attr)
{
	if(name == "drumkit")
	{
		if(attr.find("name") != attr.end())
		{
			kit._name = attr.at("name");
		}

		if(attr.find("samplerate") != attr.end())
		{
			kit._samplerate = std::stoi(attr.at("samplerate"));
		}
		else
		{
			// If 'samplerate' attribute is missing, assume 44k1Hz
			// TODO: Ask instrument what samplerate is in the audiofiles...
			kit._samplerate = 44100;
		}

		if(attr.find("description") != attr.end())
		{
			kit._description = attr.at("description");
		}

		if(attr.find("version") != attr.end())
		{
			try
			{
				kit._version = VersionStr(attr.at("version"));
			}
			catch(const char *err)
			{
				ERR(kitparser, "Error parsing version number: %s, using 1.0\n", err);
				kit._version = VersionStr(1,0,0);
			}
		}
		else
		{
			WARN(kitparser, "Missing version number, assuming 1.0\n");
			kit._version = VersionStr(1,0,0);
		}
	}

	if(name == "channels")
	{

	}

	if(name == "channel")
	{
		if(attr.find("name") == attr.end())
		{
			ERR(kitparser, "Missing channel name.\n");
			return;
		}

		Channel c(attr.at("name"));
		c.num = kit.channels.size();
		kit.channels.push_back(c);
	}

	if(name == "instruments")
	{

	}

	if(name == "instrument")
	{
		if(attr.find("name") == attr.end())
		{
			ERR(kitparser, "Missing name in instrument tag.\n");
			return;
		}

		if(attr.find("file") == attr.end())
		{
			ERR(kitparser, "Missing file in instrument tag.\n");
			return;
		}

		instr_name = attr.at("name");
		instr_file = attr.at("file");
		if(attr.find("group") != attr.end())
		{
			instr_group = attr.at("group");
		}
		else
		{
			instr_group = "";
		}
	}

	if(name == "channelmap")
	{
		if(attr.find("in") == attr.end())
		{
			ERR(kitparser, "Missing 'in' in channelmap tag.\n");
			return;
		}

		if(attr.find("out") == attr.end())
		{
			ERR(kitparser, "Missing 'out' in channelmap tag.\n");
			return;
		}

		channelmap[attr.at("in")] = attr.at("out");
	}
}

void DrumKitParser::endTag(const std::string& name)
{
	if(name == "instrument")
	{
		Instrument* instrument = new Instrument(settings);
		instrument->setGroup(instr_group);

		InstrumentParser parser(*instrument);
		parser.parseFile(path + "/" + instr_file);

		// Transfer ownership to the DrumKit object.
		kit.instruments.push_back(instrument);

		// Assign kit channel numbers to instruments channels.
		std::vector<InstrumentChannel*>::iterator ic = parser.channellist.begin();
		while(ic != parser.channellist.end())
		{
			InstrumentChannel* c = *ic;

			std::string cname = c->name;
			if(channelmap.find(cname) != channelmap.end())
			{
				cname = channelmap[cname];
			}

			for(std::size_t cnt = 0; cnt < kit.channels.size(); cnt++)
			{
				if(kit.channels[cnt].name == cname)
				{
					c->num = kit.channels[cnt].num;
				}
			}

			if(c->num == NO_CHANNEL)
			{
				ERR(kitparser, "Missing channel '%s' in instrument '%s'\n",
				      c->name.c_str(), instrument->getName().c_str());
			}
			else
			{
				/*
				  DEBUG(kitparser, "Assigned channel '%s' to number %d in instrument '%s'\n",
				  c->name.c_str(), c->num, i.name().c_str());
				*/
			}
			ic++;
		}

		channelmap.clear();
	}
}

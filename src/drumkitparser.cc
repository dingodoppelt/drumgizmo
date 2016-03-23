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
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
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

DrumKitParser::DrumKitParser(const std::string& file, DrumKit& k)
	: kit(k)
 	, refs(REFSFILE)
{
	std::string kitfile = file;

	if(refs.load())
	{
		if(file.size() > 1 && file[0] == '@')
		{
			kitfile = refs.getValue(file.substr(1));
		}
	}
	else
	{
		ERR(drumkitparser, "Error reading refs.conf");
	}

	//  instr = NULL;
	path = getPath(kitfile);

	fd = fopen(kitfile.c_str(), "r");

	//  DEBUG(kitparser, "Parsing drumkit in %s\n", kitfile.c_str());

	if(!fd)
	{
		return;
	}

	kit._file = file;
}

DrumKitParser::~DrumKitParser()
{
	if(fd)
	{
		fclose(fd);
	}

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
		Instrument* i = new Instrument();
		i->setGroup(instr_group);
		//    Instrument &i = kit.instruments[kit.instruments.size() - 1];
		InstrumentParser parser(path + "/" + instr_file, *i);
		parser.parse();
		kit.instruments.push_back(i);

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
				      c->name.c_str(), i->name().c_str());
			}
			else {
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

int DrumKitParser::readData(std::string& data, std::size_t size)
{
	if(!fd)
	{
		return -1;
	}

	data.resize(size);
	auto nr_of_bytes_read = fread((void*)data.data(), 1, size, fd);
	data.resize(nr_of_bytes_read);
	return nr_of_bytes_read;
	return fread((char*)data.c_str(), 1, size, fd);
}

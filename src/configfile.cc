/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            configfile.cc
 *
 *  Thu May 14 14:51:39 CEST 2015
 *  Copyright 2015 Bent Bisballe Nyeng
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
#include "configfile.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifdef WIN32
#include <direct.h>
#include <windows.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#else
#endif

#include <hugin.hpp>

#ifdef WIN32
#define SEP "\\"
#else
#define SEP "/"
#endif

#define CONFIGDIRNAME ".drumgizmo"

/**
 * Return the path containing the config files.
 */
static std::string getConfigPath()
{
#ifdef WIN32
	std::string configpath;
	TCHAR szPath[256];
	if(SUCCEEDED(SHGetFolderPath(
	       NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, szPath)))
	{
		configpath = szPath;
	}
#else
	std::string configpath = getenv("HOME");
#endif
	configpath += SEP;
	configpath += CONFIGDIRNAME;

	return configpath;
}

/**
 * Calling this makes sure that the config path exists
 */
static bool createConfigPath()
{
	std::string configpath = getConfigPath();

	struct stat st;
	if(stat(configpath.c_str(), &st) == 0)
	{
		DEBUG(configfile, "No configuration exists, creating directory '%s'\n",
		    configpath.c_str());
#ifdef WIN32
		if(mkdir(configpath.c_str()) < 0)
		{
#else
		if(mkdir(configpath.c_str(), 0755) < 0)
		{
#endif
			DEBUG(configfile, "Could not create config directory\n");
		}

		return false;
	}

	return true;
}

ConfigFile::ConfigFile(std::string const& filename)
	: filename(filename)
	, fp(nullptr)
{
}

ConfigFile::~ConfigFile()
{
	if (fp != nullptr)
	{
		DEBUG(configfile, "File has not been closed by the client...\n";
	}
}

bool ConfigFile::load()
{
	DEBUG(configfile, "Loading config file...\n");
	if(!open("r"))
	{
		return false;
	}

	values.clear();

	std::string line;
	while(true)
	{
		line = readLine();

		if(line == "")
			break;

		if(!parseLine(line))
		{
			return false;
		}
	}

	close();

	return true;
}

bool ConfigFile::save()
{
	DEBUG(configfile, "Saving configuration...\n");

	createConfigPath();

	if(!open("w"))
	{
		return false;
	}

	std::map<std::string, std::string>::iterator v = values.begin();
	for(; v != values.end(); ++v)
	{
		fprintf(fp, "%s:%s\n", v->first.c_str(), v->second.c_str());
	}

	close();

	return true;
}

std::string ConfigFile::getValue(const std::string& key) const
{
	auto i = values.find(key);
	if (i != values.end())
	{
		return i->second;
	}

	return "";
}

void ConfigFile::setValue(const std::string& key, const std::string& value)
{
	values[key] = value;
}

bool ConfigFile::open(std::string mode)
{
	if(fp)
	{
		close();
	}

	std::string configpath = getConfigPath();

	std::string configfile = configpath;
	configfile += SEP;
	configfile += filename;

	DEBUG(configfile, "Opening config file '%s'\n", configfile.c_str());
	fp = fopen(configfile.c_str(), mode.c_str());

	if(!fp)
	{
		return false;
	}

	return true;
}

void ConfigFile::close()
{
	fclose(fp);
	fp = nullptr;
}

std::string ConfigFile::readLine()
{
	if(!fp)
	{
		return "";
	}
	
	std::string line;

	char buf[1024];
	while(!feof(fp))
	{
		char* s = fgets(buf, sizeof(buf), fp);
		if(s)
		{
			line += buf;
			if(buf[strlen(buf) - 1] == '\n')
				break;
		}
	}

	return line;
}

bool ConfigFile::parseLine(const std::string& line)
{
	std::string key;
	std::string value;
	enum
	{
		before_key,
		in_key,
		after_key,
		before_value,
		in_value,
		in_value_single_quoted,
		in_value_double_quoted,
		after_value,
	} state = before_key;

	for(std::size_t p = 0; p < line.size(); ++p)
	{
		switch(state)
		{
		case before_key:
			if(line[p] == '#')
			{
				// Comment: Ignore line.
				p = line.size();
				continue;
			}
			if(std::isspace(line[p]))
			{
				continue;
			}
			key += line[p];
			state = in_key;
			break;

		case in_key:
			if(std::isspace(line[p]))
			{
				state = after_key;
				continue;
			}
			if(line[p] == ':' || line[p] == '=')
			{
				state = before_value;
				continue;
			}
			key += line[p];
			break;

		case after_key:
			if(std::isspace(line[p]))
			{
				continue;
			}
			if(line[p] == ':' || line[p] == '=')
			{
				state = before_value;
				continue;
			}
			ERR(configfile,
			    "Bad symbol."
			    " Expecting only whitespace or key/value seperator: '%s'",
			    line.c_str());
			return false;

		case before_value:
			if(std::isspace(line[p]))
			{
				continue;
			}
			if(line[p] == '\'')
			{
				state = in_value_single_quoted;
				continue;
			}
			if(line[p] == '"')
			{
				state = in_value_double_quoted;
				continue;
			}
			value += line[p];
			state = in_value;
			break;

		case in_value:
			if(std::isspace(line[p]))
			{
				state = after_value;
				continue;
			}
			if(line[p] == '#')
			{
				// Comment: Ignore the rest of the line.
				p = line.size();
				state = after_value;
				continue;
			}
			value += line[p];
			break;

		case in_value_single_quoted:
			if(line[p] == '\'')
			{
				state = after_value;
				continue;
			}
			value += line[p];
			break;

		case in_value_double_quoted:
			if(line[p] == '"')
			{
				state = after_value;
				continue;
			}
			value += line[p];
			break;

		case after_value:
			if(std::isspace(line[p]))
			{
				continue;
			}
			if(line[p] == '#')
			{
				// Comment: Ignore the rest of the line.
				p = line.size();
				continue;
			}
			ERR(configfile,
			    "Bad symbol."
			    " Expecting only whitespace or key/value seperator: '%s'",
			    line.c_str());
			return false;
		}
	}

	if(state == before_key)
	{
		// Line did not contain any data (empty or comment)
		return true;
	}

	// If state == in_value_XXX_quoted here, the string was not terminated.
	if(state != after_value && state != in_value)
	{
		ERR(configfile, "Malformed line: '%s'", line.c_str());
		return false;
	}

	DEBUG(configfile, "key['%s'] value['%s']\n", key.c_str(), value.c_str());

	if(key != "")
	{
		values[key] = value;
	}

	return true;
}

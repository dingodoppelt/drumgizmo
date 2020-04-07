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

#include <sys/stat.h>

#include "platform.h"

#if DG_PLATFORM == DG_PLATFORM_WINDOWS
#include <direct.h>
#include <windows.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#else
#endif

#include <hugin.hpp>

#if DG_PLATFORM == DG_PLATFORM_WINDOWS
static const std::string SEP = "\\";
#else
static const std::string SEP = "/";
#endif

#if DG_PLATFORM == DG_PLATFORM_WINDOWS
static const std::string CONFIGDIRNAME = "DrumGizmo";
#else
static const std::string CONFIGDIRNAME = ".drumgizmo";
#endif

namespace
{

/**
 * Return the path containing the config files.
 */
std::string getConfigPath()
{
	std::string configpath;
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	TCHAR szPath[256];
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, szPath)))
	{
		configpath = szPath;
	}
#else
	configpath = getenv("HOME");
#endif
	configpath += SEP;
	configpath += CONFIGDIRNAME;

	return configpath;
}

/**
 * Calling this makes sure that the config path exists
 */
bool createConfigPath()
{
	std::string const configpath = getConfigPath();

	struct stat st;
	if(stat(configpath.c_str(), &st) != 0)
	{
		DEBUG(configfile, "No configuration exists, creating directory '%s'\n",
		      configpath.c_str());
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
		if(mkdir(configpath.c_str()) < 0)
#else
		if(mkdir(configpath.c_str(), 0755) < 0)
#endif
		{
			DEBUG(configfile, "Could not create config directory\n");
		}

		return false;
	}

	return true;
}

} // end anonymous namespace

ConfigFile::ConfigFile(std::string const& filename)
	: filename(filename)
{
}

ConfigFile::~ConfigFile()
{
	if (current_file.is_open())
	{
		DEBUG(configfile, "File has not been closed by the client...\n");
		current_file.close();
	}
}

bool ConfigFile::load()
{
	DEBUG(configfile, "Loading config file...\n");
	if(!open(std::ios_base::in))
	{
		return false;
	}

	values.clear();

	std::string line;
	while(std::getline(current_file, line) && line != "")
	{
		if(!parseLine(line))
		{
			return false;
		}
	}

	current_file.close();

	return true;
}

bool ConfigFile::save()
{
	DEBUG(configfile, "Saving configuration...\n");

	createConfigPath();

	if(!open(std::ios_base::out))
	{
		return false;
	}

	for(auto const& value: values)
	{
		current_file << value.first << ":" << value.second << std::endl;
	}
	current_file.close();

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

bool ConfigFile::open(std::ios_base::openmode mode)
{
	if(current_file.is_open())
	{
		current_file.close();
	}

	std::string filename = getConfigPath();
	filename += SEP;
	filename += filename;

	DEBUG(configfile, "Opening config file '%s'\n", filename.c_str());
	current_file.open(filename, mode);

	return current_file.is_open();
}

bool ConfigFile::parseLine(const std::string& line)
{
	enum State
	{
		before_key,
		in_key,
		after_key,
		before_value,
		in_value,
		in_value_single_quoted,
		in_value_double_quoted,
		after_value,
	};

	std::string key;
	std::string value;
	State state = before_key;

	for(std::size_t pos = 0; pos < line.size(); ++pos)
	{
		auto c = line[pos];
		switch(state)
		{
		case before_key:
			if(c == '#')
			{
				// Comment: Ignore line.
				pos = line.size();
				continue;
			}
			if(std::isspace(c))
			{
				continue;
			}
			key += c;
			state = in_key;
			break;

		case in_key:
			if(std::isspace(c))
			{
				state = after_key;
				continue;
			}
			if(c == ':' || c == '=')
			{
				state = before_value;
				continue;
			}
			key += c;
			break;

		case after_key:
			if(std::isspace(c))
			{
				continue;
			}
			if(c == ':' || c == '=')
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
			if(std::isspace(c))
			{
				continue;
			}
			if(c == '\'')
			{
				state = in_value_single_quoted;
				continue;
			}
			if(c == '"')
			{
				state = in_value_double_quoted;
				continue;
			}
			value += c;
			state = in_value;
			break;

		case in_value:
			if(std::isspace(c))
			{
				state = after_value;
				continue;
			}
			if(c == '#')
			{
				// Comment: Ignore the rest of the line.
				pos = line.size();
				state = after_value;
				continue;
			}
			value += c;
			break;

		case in_value_single_quoted:
			if(c == '\'')
			{
				state = after_value;
				continue;
			}
			value += c;
			break;

		case in_value_double_quoted:
			if(c == '"')
			{
				state = after_value;
				continue;
			}
			value += c;
			break;

		case after_value:
			if(std::isspace(c))
			{
				continue;
			}
			if(c == '#')
			{
				// Comment: Ignore the rest of the line.
				pos = line.size();
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
	// If state == before_value it means that the value is empty.
	if(state != after_value && state != in_value && state != before_value)
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

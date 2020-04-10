/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            configfile.h
 *
 *  Thu May 14 14:51:38 CEST 2015
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
#pragma once

#include <string>
#include <map>
#include <fstream>

class ConfigFile
{
public:
	ConfigFile(const std::string& filename);
	virtual ~ConfigFile();

	virtual bool load();
	virtual bool save();

	virtual std::string getValue(const std::string& key) const;
	virtual void setValue(const std::string& key, const std::string& value);

protected:
	std::map<std::string, std::string> values;
	std::string filename;
	std::fstream current_file;

	// TODO: Does this have to be virtual?
	// Response: This is actually the only member function that has to be virtual.
	// I find it very ugly that we have any virtual functions in here, but it
	// seems that they were made virtual such that we can test this class properly.
	// What do you think? Test differently and make it all non-virtual?
	virtual bool open(std::ios_base::openmode mode);
	std::string readLine();
	bool parseLine(const std::string& line);
};

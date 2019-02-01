/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            pluginconfig.cc
 *
 *  Tue Jun  3 13:54:05 CEST 2014
 *  Copyright 2014 Jonas Suhr Christensen
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
#include "pluginconfig.h"

#include <hugin.hpp>

#define CONFIGFILENAME "plugingui.conf"

namespace GUI
{

Config::Config()
	: ConfigFile(CONFIGFILENAME)
{
	load();
}

Config::~Config()
{
	save();
}

bool Config::load()
{
	defaultKitPath.clear();

	if(!ConfigFile::load())
	{
		return false;
	}

	defaultKitPath = getValue("defaultKitPath");

	return true;
}

bool Config::save()
{
	setValue("defaultKitPath", defaultKitPath);

	return ConfigFile::save();
}

} // GUI::

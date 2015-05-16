/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            config.cc
 *
 *  Tue Jun  3 13:54:05 CEST 2014
 *  Copyright 2014 Jonas Suhr Christensen
 *  jsc@umbraculum.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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
#include "pluginconfig.h"

#include <hugin.hpp>
  
#define CONFIGFILENAME "plugingui.conf"

Config::Config()
  : ConfigFile(CONFIGFILENAME)
{
}

Config::~Config()
{
}

void Config::load()
{
  lastkit.clear();
  lastmidimap.clear();

  ConfigFile::load();

  lastkit = getValue("lastkit");
  lastmidimap = getValue("lastmidimap");
}

void Config::save()
{
  setValue("lastkit", lastkit);
  setValue("lastmidimap", lastmidimap);

  ConfigFile::save();
}

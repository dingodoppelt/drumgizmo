/* -*- Mode: c++ -*- */
/***************************************************************************
 *            drumgizmoconf.cc
 *
 *  Sat May  2 09:18:32 CEST 2020
 *  Copyright 2020 André Nusser
 *  andre.nusser@googlemail.com
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
#include "drumgizmoconf.h"

#define CONFIGFILENAME "drumgizmo.conf"

DrumgizmoConfig::DrumgizmoConfig()
	: ConfigFile(CONFIGFILENAME)
{
	load();
}

DrumgizmoConfig::~DrumgizmoConfig()
{
	save();
}

bool DrumgizmoConfig::load()
{
	defaultKit.clear();
	defaultMidimap.clear();

	if(!ConfigFile::load())
	{
		return false;
	}

	defaultKit = getValue("defaultKit");
	defaultMidimap = getValue("defaultMidimap");

	return true;
}

bool DrumgizmoConfig::save()
{
	setValue("defaultKit", defaultKit);
	setValue("defaultMidimap", defaultMidimap);

	return ConfigFile::save();
}

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumkit.cc
 *
 *  Wed Mar  9 15:27:27 CET 2011
 *  Copyright 2011 Bent Bisballe Nyeng
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
#include "drumkit.h"

DrumKit::DrumKit()
{
	magic = this;
}

DrumKit::~DrumKit()
{
	magic = NULL;
	clear();
}

void DrumKit::clear()
{
	instruments.clear();

	channels.clear();

	_name = "";
	_description = "";
	_samplerate = 44100;
}

bool DrumKit::isValid() const
{
	return this == magic;
}

std::string DrumKit::getFile() const
{
	return _file;
}

std::string DrumKit::getName() const
{
	return _name;
}

std::string DrumKit::getDescription() const
{
	return _description;
}

size_t DrumKit::getSamplerate() const
{
	return _samplerate;
}

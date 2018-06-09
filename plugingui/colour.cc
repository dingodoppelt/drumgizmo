/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            colour.cc
 *
 *  Fri Oct 14 09:38:28 CEST 2011
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
#include "colour.h"

#include <cstring>

namespace GUI {

Colour::Colour()
{
	data = {1.0f, 1.0f, 1.0f, 1.0f};
}

Colour::Colour(float grey, float a)
{
	data = {grey, grey, grey, a};
}

Colour::Colour(float r, float g, float b, float a)
{
	data = {r, g, b, a};
}

Colour::Colour(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a)
	: Colour(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f)
{}

Colour::Colour(const Colour& other)
{
	data = other.data;
}

Colour& Colour::operator=(const Colour& other)
{
	data = other.data;
}

bool Colour::operator==(const Colour& other) const
{
	return data == other.data;
}

bool Colour::operator!=(const Colour& other) const
{
	return !(*this == other);
}

} // GUI::

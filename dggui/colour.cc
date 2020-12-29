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

namespace GUI
{

Colour::Colour()
{
}

Colour::Colour(float grey, float a)
	: pixel({{(std::uint8_t)(grey * 255),
	          (std::uint8_t)(grey * 255),
	          (std::uint8_t)(grey * 255),
	          (std::uint8_t)(a * 255)}})
{
}

Colour::Colour(float r, float g, float b, float a)
	: pixel({{(std::uint8_t)(r * 255),
	          (std::uint8_t)(g * 255),
	          (std::uint8_t)(b * 255),
	          (std::uint8_t)(a * 255)}})
{
}

Colour::Colour(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a)
	: pixel({{r, g, b, a}})
{
}

Colour::Colour(const Colour& other)
	: pixel(other.pixel)
{
}

Colour& Colour::operator=(const Colour& other)
{
	pixel = other.pixel;
	return *this;
}

bool Colour::operator==(const Colour& other) const
{
	return pixel[0] == other.pixel[0] &&
	       pixel[1] == other.pixel[1] &&
	       pixel[2] == other.pixel[2];
}

bool Colour::operator!=(const Colour& other) const
{
	return !(*this == other);
}

} // GUI::

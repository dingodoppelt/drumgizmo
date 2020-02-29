/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            colour.h
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
#pragma once

#include <array>
#include <cstdint>

namespace GUI
{

class Colour
{
public:
	Colour();
	Colour(float grey, float alpha = 1.0f);
	Colour(float red, float green, float blue, float alpha = 1.0f);
	Colour(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a);
	Colour(const Colour& other);

	Colour& operator=(const Colour& other);

	bool operator==(const Colour& other) const;
	bool operator!=(const Colour& other) const;

	inline std::uint8_t red() const { return pixel[0]; }
	inline std::uint8_t green() const { return pixel[1]; }
	inline std::uint8_t blue() const { return pixel[2]; }
	inline std::uint8_t alpha() const { return pixel[3]; }

	std::uint8_t* data() { return pixel.data(); }
	const std::uint8_t* data() const { return pixel.data(); }

private:
	std::array<std::uint8_t, 4> pixel{{255, 255, 255, 255}};
};

} // GUI::

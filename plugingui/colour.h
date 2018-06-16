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

class Colour {
public:
	Colour();
	Colour(float grey, float alpha = 1.0f);
	Colour(float red, float green, float blue, float alpha = 1.0f);
	Colour(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a);
	Colour(const Colour& other);

	Colour& operator=(const Colour& other);

	bool operator==(const Colour& other) const;
	bool operator!=(const Colour& other) const;

	inline float red() const { return data[0]; }
	inline float green() const { return data[1]; }
	inline float blue() const { return data[2]; }
	inline float alpha() const { return data[3]; }

private:
	std::array<float, 4> data;
};

} // GUI::

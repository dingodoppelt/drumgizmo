/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drawable.h
 *
 *  Sat Jun  4 21:39:38 CEST 2016
 *  Copyright 2016 Bent Bisballe Nyeng
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

#include <cstdlib>
#include <cstdint>

namespace dggui
{

class Colour;

class Drawable
{
public:
	virtual ~Drawable() = default;

	virtual std::size_t width() const = 0;
	virtual std::size_t height() const = 0;

	virtual const Colour& getPixel(std::size_t x, std::size_t y) const = 0;
	virtual const std::uint8_t* line(std::size_t y,
	                                 std::size_t x_offset = 0) const = 0;

	virtual bool hasAlpha() const = 0;
};

} // dggui::

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            texture.h
 *
 *  Sat Jun  4 21:18:11 CEST 2016
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

#include <string>
#include <limits>

#include "imagecache.h"
#include "image.h"

namespace GUI
{

class Texture
	: public ScopedImageBorrower
	, public Drawable
{
public:
	Texture(ImageCache& image_cache, const std::string& filename,
	        std::size_t x = 0, std::size_t y = 0,
	        std::size_t width = std::numeric_limits<std::size_t>::max(),
	        std::size_t height = std::numeric_limits<std::size_t>::max());

	size_t width() const override;
	size_t height() const override;

	const Colour& getPixel(size_t x, size_t y) const override;
	const std::uint8_t* line(std::size_t y) const override;
	bool hasAlpha() const override;

private:
	std::size_t _x;
	std::size_t _y;
	std::size_t _width;
	std::size_t _height;
	Colour outOfRange{0.0f, 0.0f, 0.0f, 0.0f};
};

} // GUI::

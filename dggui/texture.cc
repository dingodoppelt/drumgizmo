/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            texture.cc
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
#include "texture.h"

namespace dggui
{

Texture::Texture(ImageCache& image_cache, const std::string& filename,
                 std::size_t x, std::size_t y,
                 std::size_t width, std::size_t height)
	: ScopedImageBorrower(image_cache, filename)
	, _x(x)
	, _y(y)
	, _width(width>image.width()?image.width():width)
	, _height(height>image.height()?image.height():height)
{
}

size_t Texture::width() const
{
	return _width;
}

size_t Texture::height() const
{
	return _height;
}

const Colour& Texture::getPixel(size_t x, size_t y) const
{
	if(x > _width || y > _height)
	{
		return outOfRange;
	}
	return image.getPixel(x + _x, y + _y);
}

const std::uint8_t* Texture::line(std::size_t y, std::size_t x_offset) const
{
	return image.line(y + _y) + _x * 4 + x_offset * 4;
}

bool Texture::hasAlpha() const
{
	return true;
}

} // dggui::

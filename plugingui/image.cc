/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            image.cc
 *
 *  Sat Mar 16 15:05:09 CET 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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
#include "image.h"

#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <cassert>

#include <hugin.hpp>

#include "resource.h"
#include "lodepng/lodepng.h"

namespace GUI
{

Image::Image(const char* data, size_t size)
{
	load(data, size);
}

Image::Image(const std::string& filename)
{
	Resource rc(filename);
	load(rc.data(), rc.size());
}

Image::Image(Image&& other)
	: _width(other._width)
	, _height(other._height)
	, image_data(std::move(other.image_data))
{
	other._width = 0;
	other._height = 0;
}

Image::~Image()
{
}

Image& Image::operator=(Image&& other)
{
	image_data.clear();
	image_data = std::move(other.image_data);
	_width = other._width;
	_height = other._height;

	other._width = 0;
	other._height = 0;

	return *this;
}

void Image::setError()
{
	Resource rc(":png_error");

	const unsigned char* ptr = (const unsigned char*)rc.data();

	std::uint32_t iw, ih;

	std::memcpy(&iw, ptr, sizeof(uint32_t));
	ptr += sizeof(uint32_t);

	std::memcpy(&ih, ptr, sizeof(uint32_t));
	ptr += sizeof(uint32_t);

	_width = iw;
	_height = ih;

	image_data.clear();
	image_data.reserve(_width * _height);

	for(std::size_t y = 0; y < _height; ++y)
	{
		for(std::size_t x = 0; x < _width; ++x)
		{
			image_data.emplace_back(Colour{ptr[0] / 255.0f, ptr[1] / 255.0f,
						ptr[2] / 255.0f, ptr[3] / 255.0f});
		}
	}

	assert(image_data.size() == (_width * _height));
}

void Image::load(const char* data, size_t size)
{
	unsigned int iw{0}, ih{0};
	unsigned char* char_image_data{nullptr};
	unsigned int res = lodepng_decode32((unsigned char**)&char_image_data,
	                                    &iw, &ih,
	                                    (const unsigned char*)data, size);

	if(res != 0)
	{
		ERR(image, "Error in lodepng_decode32: %d", res);
		setError();
		return;
	}

	_width = iw;
	_height = ih;

	image_data.clear();
	image_data.reserve(_width * _height);

	for(std::size_t y = 0; y < _height; ++y)
	{
		for(std::size_t x = 0; x < _width; ++x)
		{
			unsigned char* ptr = &char_image_data[(x + y * _width) * 4];
			image_data.emplace_back(Colour{ptr[0] / 255.0f, ptr[1] / 255.0f,
						ptr[2] / 255.0f, ptr[3] / 255.0f});
		}
	}

	assert(image_data.size() == (_width * _height));

	std::free(char_image_data);
}

size_t Image::width() const
{
	return _width;
}

size_t Image::height() const
{
	return _height;
}

const Colour& Image::getPixel(size_t x, size_t y) const
{
	if(x > _width || y > _height)
	{
		return out_of_range;
	}

	return image_data[x + y * _width];
}

} // GUI::

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

namespace dggui
{

Image::Image(const char* data, size_t size)
{
	load(data, size);
}

Image::Image(const std::string& filename)
	: filename(filename)
{
	Resource rc(filename);
	if(!rc.valid())
	{
		setError();
		return;
	}
	load(rc.data(), rc.size());
}

Image::Image(Image&& other)
	: _width(other._width)
	, _height(other._height)
	, image_data(std::move(other.image_data))
	, image_data_raw(std::move(other.image_data_raw))
	, filename(other.filename)
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
	image_data_raw.clear();
	image_data_raw = std::move(other.image_data_raw);
	_width = other._width;
	_height = other._height;
	valid = other.valid;

	other._width = 0;
	other._height = 0;
	other.valid = false;
	return *this;
}

void Image::setError()
{
	valid = false;
	Resource rc(":resources/png_error");
	if(!rc.valid())
	{
		_width = _height = 0u;
		return;
	}

	const unsigned char* ptr = (const unsigned char*)rc.data();

	std::uint32_t iw, ih;

	iw = (uint32_t) ptr[0] |
	     (uint32_t) ptr[1] << 8 |
	     (uint32_t) ptr[2] << 16 |
	     (uint32_t) ptr[3] << 24;
	ptr += sizeof(uint32_t);

	ih = (uint32_t) ptr[0] |
	     (uint32_t) ptr[1] << 8 |
	     (uint32_t) ptr[2] << 16 |
	     (uint32_t) ptr[3] << 24;
	ptr += sizeof(uint32_t);

	_width = iw;
	_height = ih;

	image_data.clear();
	image_data.reserve(_width * _height);

	image_data_raw.clear();
	image_data_raw.reserve(_width * _height * 4);
	memcpy(image_data_raw.data(), ptr, _height * _width);

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
	has_alpha = false;
	unsigned int iw{0}, ih{0};
	std::uint8_t* char_image_data{nullptr};
	unsigned int res = lodepng_decode32((std::uint8_t**)&char_image_data,
	                                    &iw, &ih,
	                                    (const std::uint8_t*)data, size);

	if(res != 0)
	{
		ERR(image, "Error in lodepng_decode32: %d while loading '%s'",
		    res, filename.c_str());
		setError();
		return;
	}

	_width = iw;
	_height = ih;

	image_data.clear();
	image_data.reserve(_width * _height);

	image_data_raw.clear();
	image_data_raw.reserve(_width * _height * 4);
	memcpy(image_data_raw.data(), char_image_data, _height * _width * 4);

	for(std::size_t y = 0; y < _height; ++y)
	{
		for(std::size_t x = 0; x < _width; ++x)
		{
			std::uint8_t* ptr = &char_image_data[(x + y * _width) * 4];
			image_data.emplace_back(Colour{ptr[0], ptr[1], ptr[2], ptr[3]});
			has_alpha |= ptr[3] != 0xff;
		}
	}

	assert(image_data.size() == (_width * _height));

	std::free(char_image_data);
	valid = true;
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

const std::uint8_t* Image::line(std::size_t y, std::size_t x_offset) const
{
	return image_data_raw.data() + y * _width * 4 + x_offset * 4;
}

bool Image::hasAlpha() const
{
	return has_alpha;
}

bool Image::isValid() const
{
	return valid;
}

} // dggui::

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
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "image.h"

#include <cstring>
#include <cstdint>

#include <hugin.hpp>

#include "resource.h"
#include "lodepng/lodepng.h"

namespace GUI {

Image::Image(const char* data, size_t size)
{
	load(data, size);
}

Image::Image(const std::string& filename)
{
	Resource rc(filename);
	load(rc.data(), rc.size());
}

Image::~Image()
{
	delete[] image_data;
}

void Image::setError(int err)
{
	Resource rc(":png_error");

	const unsigned char* p = (const unsigned char*)rc.data();

	std::uint32_t iw, ih;

	std::memcpy(&iw, p, sizeof(uint32_t));
	p += sizeof(uint32_t);

	std::memcpy(&ih, p, sizeof(uint32_t));
	p += sizeof(uint32_t);

	_width = iw;
	_height = ih;

	size_t image_size = rc.size() - (sizeof(iw) + sizeof(ih));
	image_data = new unsigned char[image_size];
	memcpy(image_data, p, image_size);
}

void Image::load(const char* data, size_t size)
{
	unsigned int res = lodepng_decode32((unsigned char**)&image_data,
	                                    &_width, &_height,
	                                    (const unsigned char*)data, size);

	if(res != 0)
	{
		ERR(image, "[read_png_file] Error during init_io");
		setError(3);
		return;
	}
}

size_t Image::width()
{
	return _width;
}

size_t Image::height()
{
	return _height;
}

Colour Image::getPixel(size_t x, size_t y)
{
	if(x > _width || y > _height)
	{
		return Colour(0,0,0,0);
	}

	unsigned char* ptr = &image_data[(x + y * width()) * 4];

	float r = ptr[0];
	float g = ptr[1];
	float b = ptr[2];
	float a = ptr[3];

	Colour c(r / 255.0, g / 255.0, b / 255.0, a / 255.0);

	return c;
}

} // GUI::

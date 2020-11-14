/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            texturedbox.cc
 *
 *  Sun Jun  5 12:22:15 CEST 2016
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
#include "texturedbox.h"

#include <cassert>

namespace GUI
{

TexturedBox::TexturedBox(ImageCache& image_cache, const std::string& filename,
                         std::size_t x0, std::size_t y0,
                         std::size_t dx1, std::size_t dx2, std::size_t dx3,
                         std::size_t dy1, std::size_t dy2, std::size_t dy3)
	: seg_a(image_cache, filename, x0            , y0            , dx1, dy1)
	, seg_b(image_cache, filename, x0 + dx1      , y0            , dx2, dy1)
	, seg_c(image_cache, filename, x0 + dx1 + dx2, y0            , dx3, dy1)
	, seg_d(image_cache, filename, x0            , y0 + dy1      , dx1, dy2)
	, seg_e(image_cache, filename, x0 + dx1      , y0 + dy1      , dx2, dy2)
	, seg_f(image_cache, filename, x0 + dx1 + dx2, y0 + dy1      , dx3, dy2)
	, seg_g(image_cache, filename, x0            , y0 + dy1 + dy2, dx1, dy3)
	, seg_h(image_cache, filename, x0 + dx1      , y0 + dy1 + dy2, dx2, dy3)
	, seg_i(image_cache, filename, x0 + dx1 + dx2, y0 + dy1 + dy2, dx3, dy3)
	, dx1(dx1)
	, dx2(dx2)
	, dx3(dx3)
	, dy1(dy1)
	, dy2(dy2)
	, dy3(dy3)
	, _width(dx1 + dx2 + dx3)
	, _height(dy1 + dy2 + dy3)
{
}

std::size_t TexturedBox::width() const
{
	return _width;
}

std::size_t TexturedBox::height() const
{
	return _height;
}

void TexturedBox::setSize(std::size_t width, std::size_t height)
{
	_width = width;
	_height = height;
}

const Colour& TexturedBox::getPixel(std::size_t x, std::size_t y) const
{
	assert(x < _width);
	assert(y < _height);

	if(y < dy1) // row 1
	{
		if(x < dx1) // col 1
		{
			return seg_a.getPixel(x, y);
		}
		else if(x < (_width - dx3)) // col 2
		{
			float scale = (float)(x - dx1) / (float)(_width - dx1 - dx3);
			assert(seg_b.width() == dx2);
			return seg_b.getPixel(scale * dx2, y);
		}
		else // col 3
		{
			return seg_c.getPixel(x - (_width - dx3), y);
		}
	}
	else if(y < (_height - dy3)) // row 2
	{
		if(x < dx1) // col 1
		{
			// TODO: Apply vertical scale
			float scale = (float)(y - dy1) / (float)(_height - dy1 - dy3);
			return seg_d.getPixel(x, scale * dy2);
		}
		else if(x < (_width - dx3)) // col 2
		{
			float scale_x = (float)(x - dx1) / (float)(_width - dx1 - dx3);
			float scale_y = (float)(y - dy1) / (float)(_height - dy1 - dy3);
			return seg_e.getPixel(scale_x * dx2, scale_y * dy2);
		}
		else // col 3
		{
			float scale = (float)(y - dy1) / (float)(_height - dy1 - dy3);
			return seg_f.getPixel(x - (_width - dx3), scale * dy2);
		}
	}
	else // row 3
	{
		if(x < dx1) // col 1
		{
			return seg_g.getPixel(x, y - (_height - dy3));
		}
		else if(x < (_width - dx3)) // col 2
		{
			float scale = (float)(x - dx1) / (float)(_width - dx1 - dx3);
			return seg_h.getPixel(scale * dx2, y - (_height - dy3));
		}
		else // col 3
		{
			return seg_i.getPixel(x - (_width - dx3), y - (_height - dy3));
		}
	}

	return outOfRange;
}

const std::uint8_t* TexturedBox::line(std::size_t y, std::size_t x_offset) const
{
	// TODO: Gather line into temporary buffer?
	return nullptr;
}

bool TexturedBox::hasAlpha() const
{
	return true;
}

} // GUI::

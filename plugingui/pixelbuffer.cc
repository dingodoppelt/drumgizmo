/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            pixelbuffer.cc
 *
 *  Thu Nov 10 09:00:38 CET 2011
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
#include "pixelbuffer.h"

#include <cassert>

#include <cstdlib>
#include <cstring>

namespace GUI
{

PixelBuffer::PixelBuffer(std::size_t width, std::size_t height)
	: buf(nullptr)
{
	realloc(width, height);
}

PixelBuffer::~PixelBuffer()
{
	free(buf);
}

void PixelBuffer::realloc(std::size_t width, std::size_t height)
{
	free(buf);
	buf = (std::uint8_t *)calloc(width * height, 3);
	this->width = width;
	this->height = height;
}

#define PX(k) ((x + y * width) * 3 + k)
void PixelBuffer::setPixel(std::size_t x, std::size_t y, const Colour& c)
{
	if(c.alpha() == 0)
	{
		return;
	}

	if(c.alpha() < 255)
	{
		unsigned int a = c.alpha();
		unsigned int b = 255 - a;

		std::uint8_t* pixel = &buf[PX(0)];
		*pixel = (std::uint8_t)(((int)c.red()   * a + (int)*pixel * b) / 255);
		++pixel;
		*pixel = (std::uint8_t)(((int)c.green() * a + (int)*pixel * b) / 255);
		++pixel;
		*pixel = (std::uint8_t)(((int)c.blue()  * a + (int)*pixel * b) / 255);
	}
	else
	{
		memcpy(&buf[PX(0)], c.data(), 3);
	}
}

void PixelBuffer::writeLine(std::size_t x, std::size_t y,
                            const std::uint8_t* line, std::size_t len)
{
	std::uint8_t* target = &buf[PX(0)];
	while(len)
	{
		if(line[3] == 0xff)
		{
			memcpy(target, line, 3);
		}
		else
		{
			unsigned int a = line[3];
			unsigned int b = 255 - a;

			target[0] = (std::uint8_t)(((int)line[0] * a + (int)target[0] * b) / 255);
			target[1] = (std::uint8_t)(((int)line[1] * a + (int)target[1] * b) / 255);
			target[2] = (std::uint8_t)(((int)line[2] * a + (int)target[2] * b) / 255);
		}
		target += 3;
		line += 4;
		--len;
	}
}

Rect PixelBuffer::updateBuffer(std::vector<PixelBufferAlpha*>& pixel_buffers)
{
	bool has_dirty_rect{false};
	Rect dirty_rect;

	for(const auto& pixel_buffer : pixel_buffers)
	{
		if(pixel_buffer->dirty)
		{
			auto x1 = (std::size_t)pixel_buffer->x;
			auto x2 = (std::size_t)(pixel_buffer->x + pixel_buffer->width);
			auto y1 = (std::size_t)pixel_buffer->y;
			auto y2 = (std::size_t)(pixel_buffer->y + pixel_buffer->height);

			pixel_buffer->dirty = false;
			if(!has_dirty_rect)
			{
				// Insert this area:
				dirty_rect = {x1, y1, x2, y2};
				has_dirty_rect = true;
			}
			else
			{
				// Expand existing area:
				auto x1_0 = dirty_rect.x1;
				auto y1_0 = dirty_rect.y1;
				auto x2_0 = dirty_rect.x2;
				auto y2_0 = dirty_rect.y2;
				dirty_rect = {
					(x1_0 < x1) ? x1_0 : x1,
					(y1_0 < y1) ? y1_0 : y1,
					(x2_0 > x2) ? x2_0 : x2,
					(y2_0 > y2) ? y2_0 : y2
				};
			}
		}

		if(pixel_buffer->has_last)
		{
			auto x1 = (std::size_t)pixel_buffer->last_x;
			auto x2 = (std::size_t)(pixel_buffer->last_x + pixel_buffer->last_width);
			auto y1 = (std::size_t)pixel_buffer->last_y;
			auto y2 = (std::size_t)(pixel_buffer->last_y + pixel_buffer->last_height);

			pixel_buffer->has_last = false;
			if(!has_dirty_rect)
			{
				// Insert this area:
				dirty_rect = {x1, y1, x2, y2};
				has_dirty_rect = true;
			}
			else
			{
				// Expand existing area:
				auto x1_0 = dirty_rect.x1;
				auto y1_0 = dirty_rect.y1;
				auto x2_0 = dirty_rect.x2;
				auto y2_0 = dirty_rect.y2;
				dirty_rect = {
					(x1_0 < x1) ? x1_0 : x1,
					(y1_0 < y1) ? y1_0 : y1,
					(x2_0 > x2) ? x2_0 : x2,
					(y2_0 > y2) ? y2_0 : y2
				};
			}
		}
	}

	if(!has_dirty_rect)
	{
		return {};
	}

	for(const auto& pixel_buffer : pixel_buffers)
	{
		if(!pixel_buffer->visible)
		{
			continue;
		}

		int update_width = pixel_buffer->width;
		int update_height = pixel_buffer->height;

		// Skip buffer if not inside window.
		if(((int)width < pixel_buffer->x) ||
		   ((int)height < pixel_buffer->y))
		{
			continue;
		}

		if(update_width > ((int)width - pixel_buffer->x))
		{
			update_width = ((int)width - pixel_buffer->x);
		}

		if(update_height > ((int)height - pixel_buffer->y))
		{
			update_height = ((int)height - pixel_buffer->y);
		}

		auto from_x  = (int)dirty_rect.x1 - pixel_buffer->x;
		from_x = std::max(0, from_x);
		auto from_y  = (int)dirty_rect.y1 - pixel_buffer->y;
		from_y = std::max(0, from_y);

		auto to_x = (int)dirty_rect.x2 - pixel_buffer->x;
		to_x = std::min(to_x, (int)update_width);
		auto to_y = (int)dirty_rect.y2 - pixel_buffer->y;
		to_y = std::min(to_y, (int)update_height);

		if(to_x < from_x)
		{
			continue;
		}

		for(int y = from_y; y < to_y; y++)
		{
			writeLine(pixel_buffer->x + from_x,
			          pixel_buffer->y + y,
			          pixel_buffer->getLine(from_x, y),
			          to_x - from_x);
		}
	}

	dirty_rect.x2 = std::min(width, dirty_rect.x2);
	dirty_rect.y2 = std::min(height, dirty_rect.y2);

	// Make sure we don't try to paint a rect backwards.
	if(dirty_rect.x1 > dirty_rect.x2)
	{
		std::swap(dirty_rect.x1, dirty_rect.x2);
	}

	if(dirty_rect.y1 > dirty_rect.y2)
	{
		std::swap(dirty_rect.y1, dirty_rect.y2);
	}

	return dirty_rect;
}


PixelBufferAlpha::PixelBufferAlpha(std::size_t width, std::size_t height)
	: managed(true)
	, buf(nullptr)
	, x(0)
	, y(0)
{
	realloc(width, height);
}

PixelBufferAlpha::~PixelBufferAlpha()
{
	if(managed)
	{
		free(buf);
	}
}

void PixelBufferAlpha::realloc(std::size_t width, std::size_t height)
{
	free(buf);
	buf = (std::uint8_t *)calloc(width * height, 4);
	this->width = width;
	this->height = height;
}

void PixelBufferAlpha::clear()
{
	memset(buf, 0, width * height * 4);
}

#undef PX
#define PX(k) ((x + y * width) * 4 + k)
//void PixelBufferAlpha::setPixel(std::size_t x, std::size_t y,
//                                std::uint8_t red,
//                                std::uint8_t green,
//                                std::uint8_t blue,
//                                std::uint8_t alpha)
//{
//	//assert(x < width);
//	//assert(y < height);
//
//	std::uint8_t* pixel = &buf[PX(0)];
//	*pixel = red;
//	++pixel;
//	*pixel = green;
//	++pixel;
//	*pixel = blue;
//	++pixel;
//	*pixel = alpha;
//}

void PixelBufferAlpha::setPixel(std::size_t x, std::size_t y, const Colour& c)
{
	std::uint8_t* pixel = &buf[PX(0)];
	*pixel = c.red();
	++pixel;
	*pixel = c.green();
	++pixel;
	*pixel = c.blue();
	++pixel;
	*pixel = c.alpha();
}

void PixelBufferAlpha::writeLine(std::size_t x, std::size_t y,
                                 const std::uint8_t* line, std::size_t len)
{
	auto offset = &buf[PX(0)];
	if(x + y * width + len > width * height)
	{
		return; // out of bounds
	}
	std::memcpy(offset, line, len * 4);
}

// SIMD: https://github.com/WojciechMula/toys/blob/master/blend_32bpp/blend_32bpp.c

void PixelBufferAlpha::blendLine(std::size_t x, std::size_t y,
                                 const std::uint8_t* line, std::size_t len)
{
	auto offset = &buf[PX(0)];
	if(x + y * width + len > width * height)
	{
		return; // out of bounds
	}

	std::uint32_t* foreground = (std::uint32_t*)line;
	std::uint32_t* background = (std::uint32_t*)offset;
	for(std::size_t x = 0; x < len; ++x)
	{
		auto Rf =  *foreground & 0xff;
		auto Gf = (*foreground >>  8) & 0xff;
		auto Bf = (*foreground >> 16) & 0xff;
		auto Af = (*foreground >> 24) & 0xff;

		auto Rb =  *background & 0xff;
		auto Gb = (*background >>  8) & 0xff;
		auto Bb = (*background >> 16) & 0xff;
		auto Ab = (*background >> 24) & 0xff;

		auto R = (Rf * Af)/256 + Rb;
		auto G = (Gf * Af)/256 + Gb;
		auto B = (Bf * Af)/256 + Bb;

		if (R > 255) R = 255;
		if (G > 255) G = 255;
		if (B > 255) B = 255;

//		auto a = Ab / 255.0;
//		auto b = Af / 255.0;
//		b *= (1 - a);

//		(Af / 255.0 * (1 - Ab / 255.0)) * 255
		auto b = Ab * (255 - Af);

		*background = R | (G << 8) | (B << 16) | b << 24;

		++foreground;
		++background;
	}
}


// http://en.wikipedia.org/wiki/Alpha_compositing
static inline void getAlpha(std::uint8_t _a, std::uint8_t _b,
                            float &a, float &b)
{
	a = _a / 255.0;
	b = _b / 255.0;
	b *= (1 - a);
}

void PixelBufferAlpha::addPixel(std::size_t x, std::size_t y,
                                std::uint8_t red,
                                std::uint8_t green,
                                std::uint8_t blue,
                                std::uint8_t alpha)
{
	//assert(x < width);
	//assert(y < height);

	if(alpha == 0)
	{
		return;
	}

	std::uint8_t* pixel = &buf[PX(0)];

	if(alpha < 255)
	{
		float a, b;
		getAlpha(alpha, buf[PX(3)], a, b);

		*pixel = (std::uint8_t)((red   * a + *pixel * b) / (a + b));
		++pixel;
		*pixel = (std::uint8_t)((green * a + *pixel * b) / (a + b));
		++pixel;
		*pixel = (std::uint8_t)((blue  * a + *pixel * b) / (a + b));
		++pixel;
		*pixel = (a + b) * 255;
	}
	else
	{
		*pixel = red;
		++pixel;
		*pixel = green;
		++pixel;
		*pixel = blue;
		++pixel;
		*pixel = alpha;
	}
}

void PixelBufferAlpha::addPixel(std::size_t x, std::size_t y, const Colour& c)
{
	addPixel(x, y, c.red(), c.green(), c.blue(), c.alpha());
}

void PixelBufferAlpha::pixel(std::size_t x, std::size_t y,
                             std::uint8_t* red,
                             std::uint8_t* green,
                             std::uint8_t* blue,
                             std::uint8_t* alpha) const
{
	//assert(x < width);
	//assert(y < height);

	std::uint8_t* pixel = &buf[PX(0)];
	*red = *pixel;
	++pixel;
	*green = *pixel;
	++pixel;
	*blue = *pixel;
	++pixel;
	*alpha = *pixel;
}

const Colour& PixelBufferAlpha::pixel(std::size_t x, std::size_t y) const
{
	static Colour c;
	c = Colour(buf[PX(0)],  buf[PX(1)], buf[PX(2)], buf[PX(3)]);
	return c;
}

const std::uint8_t* PixelBufferAlpha::getLine(std::size_t x, std::size_t y) const
{
	return &buf[PX(0)];
}

} // GUI::

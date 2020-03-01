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

void PixelBuffer::blendLine(std::size_t x, std::size_t y,
                            const std::uint8_t* line, std::size_t len)
{
	std::uint8_t* target = buf + (x + y * width) * 3;
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

			target[0] = (std::uint8_t)((line[0] * a + target[0] * b) / 255);
			target[1] = (std::uint8_t)((line[1] * a + target[1] * b) / 255);
			target[2] = (std::uint8_t)((line[2] * a + target[2] * b) / 255);
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
			blendLine(pixel_buffer->x + from_x,
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
void PixelBufferAlpha::setPixel(std::size_t x, std::size_t y, const Colour& c)
{
	std::uint8_t* pixel = &buf[PX(0)];
	memcpy(pixel, c.data(), 4);
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
	if(x + y * width + len > width * height)
	{
		return; // out of bounds
	}

	std::uint8_t* target = &buf[PX(0)];
	for(std::size_t x = 0; x < len; ++x)
	{
		unsigned int a = line[3];
		unsigned int b = 255 - a;

		target[0] = (std::uint8_t)((line[0] * a + target[0] * b) / 255);
		target[1] = (std::uint8_t)((line[1] * a + target[1] * b) / 255);
		target[2] = (std::uint8_t)((line[2] * a + target[2] * b) / 255);
		target[3] = a * b / 255;
		line += 4;
		target += 4;
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

void PixelBufferAlpha::addPixel(std::size_t x, std::size_t y, const Colour& c)
{
	if(c.alpha() == 0)
	{
		return;
	}

	std::uint8_t* pixel = &buf[PX(0)];

	if(c.alpha() < 255)
	{
		float a, b;
		getAlpha(c.alpha(), buf[PX(3)], a, b);

		*pixel = (std::uint8_t)((c.red()   * a + *pixel * b) / (a + b));
		++pixel;
		*pixel = (std::uint8_t)((c.green() * a + *pixel * b) / (a + b));
		++pixel;
		*pixel = (std::uint8_t)((c.blue()  * a + *pixel * b) / (a + b));
		++pixel;
		*pixel = (a + b) * 255;
	}
	else
	{
		memcpy(pixel, c.data(), 4);
	}
}

const Colour& PixelBufferAlpha::pixel(std::size_t x, std::size_t y) const
{
	static Colour c;
	memcpy(c.data(), &buf[PX(0)], 4);
	return c;
}

const std::uint8_t* PixelBufferAlpha::getLine(std::size_t x, std::size_t y) const
{
	return &buf[PX(0)];
}

} // GUI::

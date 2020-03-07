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
{
	realloc(width, height);
}

PixelBuffer::~PixelBuffer()
{
}

void PixelBuffer::realloc(std::size_t width, std::size_t height)
{
	buf_data.resize(width * height * 3);
	buf = buf_data.data();
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
			std::memcpy(target, line, 3);
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
{
	realloc(width, height);
}

PixelBufferAlpha::~PixelBufferAlpha()
{
}

void PixelBufferAlpha::realloc(std::size_t width, std::size_t height)
{
	buf_data.resize(width * height * 4);
	buf = buf_data.data();
	this->width = width;
	this->height = height;
	clear();
}

void PixelBufferAlpha::clear()
{
	std::memset(buf, 0, width * height * 4);
}

void PixelBufferAlpha::setPixel(std::size_t x, std::size_t y, const Colour& c)
{
	std::uint8_t* pixel = buf + (x + y * width) * 4;
	std::memcpy(pixel, c.data(), 4);
}

void PixelBufferAlpha::writeLine(std::size_t x, std::size_t y,
                                 const std::uint8_t* line, std::size_t len)
{
	if(x >= width || y >= height)
	{
		return;
	}

	if(x + len > width)
	{
		len = width - x;
	}

	auto offset = buf + (x + y * width) * 4;

	std::memcpy(offset, line, len * 4);
}


// SIMD: https://github.com/WojciechMula/toys/blob/master/blend_32bpp/blend_32bpp.c
// Alpha blending: http://en.wikipedia.org/wiki/Alpha_compositing

void PixelBufferAlpha::blendLine(std::size_t x, std::size_t y,
                                 const std::uint8_t* line, std::size_t len)
{
	if(x >= width || y >= height)
	{
		return;
	}

	if(x + len > width)
	{
		len = width - x;
	}

	int a, b;
	std::uint8_t* target = buf + (x + y * width) * 4;
	while(len)
	{
		if(line[3] == 0xff)
		{
			const std::uint8_t* end = line;
			while(end[3] == 0xff && end < line + len * 4)
			{
				end += 4;
			}
			auto chunk_len = end - line;
			std::memcpy(target, line, chunk_len);
			line += chunk_len;
			target += chunk_len;
			len -= chunk_len / 4;
			continue;
		}
		else if(line[3] == 0)
		{
			// Do nothing
		}
		else
		{
			a = line[3];
			b = target[3] * (255 - a) / 255;

			target[0] = (line[0] * a + target[0] * b) / (a + b);
			target[1] = (line[1] * a + target[1] * b) / (a + b);
			target[2] = (line[2] * a + target[2] * b) / (a + b);
			target[3] = (int)target[3] + line[3] * (255 - target[3]) / 255;
		}

		line += 4;
		target += 4;
		--len;
	}
}

void PixelBufferAlpha::addPixel(std::size_t x, std::size_t y, const Colour& c)
{
	if(x >= width || y >= height)
	{
		return; // out of bounds
	}

	const std::uint8_t* colour = c.data();

	if(colour[3] == 0)
	{
		return;
	}

	int a, b;
	std::uint8_t* target = buf + (x + y * width) * 4;

	if(colour[3] == 0xff)
	{
		std::memcpy(target, colour, 4);
	}
	else
	{
		a = colour[3];
		b = target[3] * (255 - a) / 255;

		target[0] = (colour[0] * a + target[0] * b) / (a + b);
		target[1] = (colour[1] * a + target[1] * b) / (a + b);
		target[2] = (colour[2] * a + target[2] * b) / (a + b);
		target[3] = (int)target[3] + colour[3] * (255 - target[3]) / 255;
	}
}

const Colour& PixelBufferAlpha::pixel(std::size_t x, std::size_t y) const
{
	static Colour c;
	std::memcpy(c.data(), buf + (x + y * width) * 4, 4);
	return c;
}

const std::uint8_t* PixelBufferAlpha::getLine(std::size_t x, std::size_t y) const
{
	return buf + (x + y * width) * 4;
}

} // GUI::

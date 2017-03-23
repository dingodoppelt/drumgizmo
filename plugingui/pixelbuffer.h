/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            pixelbuffer.h
 *
 *  Thu Nov 10 09:00:37 CET 2011
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
#pragma once

#include "colour.h"

#include <cstddef>

namespace GUI
{

class PixelBuffer
{
public:
	PixelBuffer(std::size_t width, std::size_t height);
	~PixelBuffer();

	void realloc(std::size_t width, std::size_t height);

	void setPixel(std::size_t x, std::size_t y,
	              unsigned char red,
	              unsigned char green,
	              unsigned char blue,
	              unsigned char alpha);

	unsigned char* buf{nullptr};
	std::size_t width{0};
	std::size_t height{0};
};

class PixelBufferAlpha
{
public:
	PixelBufferAlpha() = default;
	PixelBufferAlpha(std::size_t width, std::size_t height);
	~PixelBufferAlpha();

	void realloc(std::size_t width, std::size_t height);

	void setPixel(std::size_t x, std::size_t y,
	              unsigned char red,
	              unsigned char green,
	              unsigned char blue,
	              unsigned char alpha);

	void addPixel(std::size_t x, std::size_t y,
	              unsigned char red,
	              unsigned char green,
	              unsigned char blue,
	              unsigned char alpha);

	void addPixel(std::size_t x, std::size_t y, const Colour& c);

	void pixel(std::size_t x, std::size_t y,
	           unsigned char* red,
	           unsigned char* green,
	           unsigned char* blue,
	           unsigned char* alpha) const;

	bool managed{false};
	unsigned char* buf{nullptr};
	std::size_t width{0};
	std::size_t height{0};
	int x{0};
	int y{0};
	bool dirty{true};
	bool visible{true};

	// Add optional dirty rect that this pixelbuffer took up since it was last
	// rendered. Make sure to update this list on resize and/or move.
	std::size_t last_width{0};
	std::size_t last_height{0};
	int last_x{0};
	int last_y{0};
	bool has_last{false};
};

} // GUI::

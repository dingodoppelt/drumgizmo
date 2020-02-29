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
#include <vector>

#include "guievent.h"

namespace GUI
{

class PixelBuffer
{
public:
	PixelBuffer(std::size_t width, std::size_t height);
	~PixelBuffer();

	void realloc(std::size_t width, std::size_t height);

	void setPixel(std::size_t x, std::size_t y, const Colour& c);

	void writeLine(std::size_t x, std::size_t y,
	               const std::uint8_t* line, std::size_t len);

	Rect updateBuffer(std::vector<class PixelBufferAlpha*>& pixel_buffers);

	std::uint8_t* buf{nullptr};
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

	void clear();

	void setPixel(std::size_t x, std::size_t y, const Colour& c);

	void writeLine(std::size_t x, std::size_t y,
	               const std::uint8_t* line, std::size_t len);
	void blendLine(std::size_t x, std::size_t y,
	               const std::uint8_t* line, std::size_t len);

	void addPixel(std::size_t x, std::size_t y,
	              std::uint8_t red,
	              std::uint8_t green,
	              std::uint8_t blue,
	              std::uint8_t alpha);

	void addPixel(std::size_t x, std::size_t y, const Colour& c);

	void pixel(std::size_t x, std::size_t y,
	           std::uint8_t* red,
	           std::uint8_t* green,
	           std::uint8_t* blue,
	           std::uint8_t* alpha) const;

	const Colour& pixel(std::size_t x, std::size_t y) const;

	const std::uint8_t* getLine(std::size_t x, std::size_t y) const;

	bool managed{false};
	std::uint8_t* buf{nullptr};
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

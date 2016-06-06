/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            texturedbox.h
 *
 *  Sun Jun  5 12:22:14 CEST 2016
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
#pragma once

#include "drawable.h"
#include "imagecache.h"
#include "texture.h"

namespace GUI
{

class TexturedBox
	: public Drawable
{
public:
	//! Draw a box from 9 image segments nested inside the same image.
	//! An image says more than a thousand words:
	//! .----------------------------------------.
	//! |  (x0, y0)                              |
	//! |      \  dx1      dx2      dx3          |
	//! |       .-----+-----------+-----.  \     |
	//! |  dy1  |  A  |  <--B-->  |  C  |   |    |
	//! |       +-----+-----------+-----+   |    |
	//! |       | /|\ |    /|\    | /|\ |   | h  |
	//! |       |  |  |     |     |  |  |   | e  |
	//! |  dy2  |  D  |  <--E-->  |  F  |   > i  |
	//! |       |  |  |     |     |  |  |   | g  |
	//! |       | \|/ |    \|/    | \|/ |   | h  |
	//! |       +-----+-----------+-----+   | t  |
	//! |  dy3  |  G  |  <--H-->  |  I  |   |    |
	//! |       `-----+-----------+-----`  /     |
	//! |                                        |
	//! |       \___________ ___________/        |
	//! |                   V                    |
	//! |                 width                  |
	//! `----------------------------------------`
	//!
	//! \param image_cache A reference to the image cache object.
	//! \param filename The filename of the texture image to use.
	//! \param (x0, y0) is coordinate of the upper left corner of the A segment.
	//! \param (width, height) is the total rendered size of the Box.
	//! \param dx1 is the width of the A, C and F segments.
	//! \param dx2 is the width of the B, E and H segments.
	//! \param dx3 is the width of the C, F and I segments.
	//! \param dy1 is the height of the A, B and C segments.
	//! \param dy2 is the height of the D, E and F segments.
	//! \param dy3 is the height of the G, G and I segments.
	//!
	//! Segments A, C, G and I are drawn with no stretch.
	//! Segments B and H are stretched horizontally to fill the
	//! gaps between A, C and G, I so that resulting width is 'width'
	//! Segments D and F are stretched vertically to fill the
	//! gaps between A, G and C, I so that resulting height is 'height'
	//! Segment E will be stretched both horizontally and vertically
	//! to fill the inner space between B, H and D, F.
	TexturedBox(ImageCache& image_cache, const std::string& filename,
	            std::size_t x0, std::size_t y0,
	            std::size_t dx1, std::size_t dx2, std::size_t dx3,
	            std::size_t dy1, std::size_t dy2, std::size_t dy3);

	// From Drawable:
	std::size_t width() const override;
	std::size_t height() const override;

	void setSize(std::size_t width, std::size_t height);

	const Colour& getPixel(std::size_t x, std::size_t y) const override;

private:
	Texture seg_a;
	Texture seg_b;
	Texture seg_c;
	Texture seg_d;
	Texture seg_e;
	Texture seg_f;
	Texture seg_g;
	Texture seg_h;
	Texture seg_i;

	std::size_t _x;
	std::size_t _y;
	std::size_t _width{100};
	std::size_t _height{100};
	std::size_t dx1;
	std::size_t dx2;
	std::size_t dx3;
	std::size_t dy1;
	std::size_t dy2;
	std::size_t dy3;

	Colour outOfRange{0.0f, 0.0f, 0.0f, 0.0f};
};

} // GUI::

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            font.cc
 *
 *  Sat Nov 12 11:13:41 CET 2011
 *  Copyright 2011 Bent Bisballe Nyeng
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
#include "font.h"

namespace GUI {

Font::Font(const std::string& fontfile)
	: img_font(fontfile)
{
	size_t px = 0;
	size_t c;

	for(c = 0; c < characters.size() && px < img_font.width(); ++c)
	{
		auto& character = characters[c];
		character.offset = px + 1;

		if(c > 0)
		{
			characters[c - 1].width =
				character.offset - characters[c - 1].offset - 1;
		}

		++px;

		Colour pixel;
		while(px < img_font.width())
		{
			pixel = img_font.getPixel(px, 0);

			// Find next purple pixel in top row:
			if((pixel.red == 1) && (pixel.green == 0) &&
			   (pixel.blue == 1) && (pixel.alpha == 1))
			{
				break;
			}

			++px;
		}

		characters[c] = character;
	}

	++c;

	characters[c - 1].width = characters[c].offset - characters[c - 1].offset - 1;
}

size_t Font::textWidth(const std::string& text) const
{
	size_t len = 0;

	for(auto cha : text)
	{
		auto& character = characters[cha];
		len += character.width + spacing + character.post_bias;
	}

	return len;
}

size_t Font::textHeight(const std::string& text) const
{
	return img_font.height();
}

void Font::setLetterSpacing(int letterSpacing)
{
	spacing = letterSpacing;
}

int Font::letterSpacing() const
{
	return spacing;
}

PixelBufferAlpha *Font::render(const std::string& text) const
{
	PixelBufferAlpha *pb =
		new PixelBufferAlpha(textWidth(text), textHeight(text));

	int x_offset = 0;
	for(auto cha : text)
	{
		auto& character = characters[cha];
		for(size_t x = 0; x < character.width; ++x)
		{
			for(size_t y = 0; y < img_font.height(); ++y)
			{
				Colour c = img_font.getPixel(x + character.offset, y);
				pb->setPixel(x + x_offset + character.pre_bias, y,
				             c.red * 255, c.green * 255, c.blue * 255, c.alpha * 255);
			}
		}
		x_offset += character.width + spacing + character.post_bias;
	}

	return pb;
}

} // GUI::

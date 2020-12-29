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
#include "font.h"

#include <cassert>

namespace GUI
{

Font::Font(const std::string& fontfile)
	: img_font(fontfile)
{
	std::size_t px = 0;
	std::size_t c;

	for(c = 0; c < characters.size() && px < img_font.width(); ++c)
	{
		auto& character = characters[c];
		character.offset = px + 1;

		if(c > 0)
		{
			assert(character.offset >= characters[c - 1].offset);
			characters[c - 1].width = character.offset - characters[c - 1].offset;
			if(characters[c].offset != characters[c - 1].offset)
			{
				--characters[c - 1].width;
			}
		}

		++px;

		while(px < img_font.width())
		{
			auto& pixel = img_font.getPixel(px, 0);

			// Find next purple pixel in top row:
			if((pixel.red() == 255) && (pixel.green() == 0) &&
			   (pixel.blue() == 255) && (pixel.alpha() == 255))
			{
				break;
			}

			++px;
		}

		characters[c] = character;
	}

	--c;

	assert(characters[c].offset >= characters[c - 1].offset);
	characters[c - 1].width = characters[c].offset - characters[c - 1].offset;
	if(characters[c].offset != characters[c - 1].offset)
	{
		--characters[c - 1].width;
	}
}

size_t Font::textWidth(const std::string& text) const
{
	size_t len = 0;

	for(unsigned char cha : text)
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
	for(std::size_t i = 0; i < text.length(); ++i)
	{
		unsigned char cha = text[i];
		auto& character = characters.at(cha);
		for(size_t x = 0; x < character.width; ++x)
		{
			for(size_t y = 0; y < img_font.height(); ++y)
			{
				auto& c = img_font.getPixel(x + character.offset, y);
				pb->setPixel(x + x_offset + character.pre_bias, y, c);
			}
		}
		x_offset += character.width + spacing + character.post_bias;
	}

	return pb;
}

} // GUI::

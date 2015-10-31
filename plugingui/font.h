/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            font.h
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
#pragma once

#include <string>
#include <array>

#include "pixelbuffer.h"
#include "image.h"

namespace GUI {

class Font {
public:
	Font(const std::string& fontfile = ":font.png");

	size_t textWidth(const std::string& text);
	size_t textHeight(const std::string& text = "");

	PixelBufferAlpha *render(const std::string& text);

private:
	Image img_font;

	struct Character {
		int offset = 0;
		size_t width = 0;
		int pre_bias = 0;
		int post_bias = 0;
	};

	std::array<Character, 255> characters;

//	std::map<char, int> character_offset;
//	std::map<char, size_t> character_width;
//	std::map<char, int> character_pre_bias;
//	std::map<char, int> character_post_bias;
};

} // GUI::

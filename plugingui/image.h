/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            image.h
 *
 *  Sat Mar 16 15:05:08 CET 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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

#include <string>
#include <vector>

#include "drawable.h"
#include "colour.h"
#include "resource.h"

namespace GUI {

class Image
	: public Drawable
{
public:
	Image(const char* data, size_t size);
	Image(const std::string& filename);
	Image(Image&& other);
	virtual ~Image();

	Image& operator=(Image&& other);

	size_t width() const override;
	size_t height() const override;

	const Colour& getPixel(size_t x, size_t y) const override;

private:
	void setError();

	void load(const char* data, size_t size);

	std::size_t _width{0};
	std::size_t _height{0};
	std::vector<Colour> image_data;
	Colour out_of_range{0.0f, 0.0f, 0.0f, 0.0f};
	std::string filename;
};

} // GUI::

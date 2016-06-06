/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            imagecache.h
 *
 *  Thu Jun  2 17:12:05 CEST 2016
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

#include <string>
#include <map>
#include <utility>

namespace GUI
{

class Image;
class ImageCache;

class ScopedImageBorrower
{
public:
	ScopedImageBorrower(ImageCache& imageCache, const std::string& filename);
	ScopedImageBorrower(ScopedImageBorrower&& other);
	virtual ~ScopedImageBorrower();

	ScopedImageBorrower& operator=(ScopedImageBorrower&& other);

	Image& operator*();
	Image& operator()();

protected:
	ImageCache& imageCache;
	std::string filename;
	Image& image;
};

class ImageCache
{
public:
	ScopedImageBorrower getImage(const std::string& filename);

private:
	friend class ScopedImageBorrower;

	Image& borrow(const std::string& filename);
	void giveBack(const std::string& filename);

protected:
	std::map<std::string, std::pair<std::size_t, Image>> imageCache;
};

} // GUI::

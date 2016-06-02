/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            imagecache.cc
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
#include "imagecache.h"

#include <cassert>

#include "image.h"

namespace GUI
{

ScopedImageBorrower::ScopedImageBorrower(ImageCache& imageCache,
                                         const std::string& filename)
	: imageCache(imageCache)
	, filename(filename)
	, image(imageCache.borrow(filename))
{
}

ScopedImageBorrower::ScopedImageBorrower(ScopedImageBorrower&& other)
	: imageCache(other.imageCache)
	, filename(other.filename)
	, image(other.image)
{
	other.filename.clear();
}

ScopedImageBorrower::~ScopedImageBorrower()
{
	if(!filename.empty())
	{
		imageCache.giveBack(filename);
	}
}

Image& ScopedImageBorrower::operator*()
{
	return image;
}

Image& ScopedImageBorrower::operator()()
{
	return image;
}

ScopedImageBorrower ImageCache::getImage(const std::string& filename)
{
	return ScopedImageBorrower(*this, filename);
}

Image& ImageCache::borrow(const std::string& filename)
{
	auto cacheIterator = imageCache.find(filename);
	if(cacheIterator == imageCache.end())
	{
		Image image(filename);
		auto insertValue =
			imageCache.emplace(filename,
			                   std::move(std::make_pair(0, std::move(image))));
		cacheIterator = insertValue.first;
	}

	auto& cacheEntry = cacheIterator->second;
	++cacheEntry.first;
	return cacheEntry.second;
}

void ImageCache::giveBack(const std::string& filename)
{
	auto cacheIterator = imageCache.find(filename);
	assert(cacheIterator != imageCache.end());
	auto& cacheEntry = cacheIterator->second;
	--cacheEntry.first;
	if(cacheEntry.first == 0)
	{
		imageCache.erase(cacheIterator);
	}
}

} // GUI::

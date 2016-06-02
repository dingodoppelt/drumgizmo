/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            imagecachetest.cc
 *
 *  Thu Jun  2 18:54:31 CEST 2016
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
#include <cppunit/extensions/HelperMacros.h>

#include <cassert>

#include <imagecache.h>
#include <image.h>

class TestableImageCache
	: public GUI::ImageCache
{
public:
	std::size_t count(const std::string& filename)
	{
		if(imageCache.find(filename) == imageCache.end())
		{
			return 0;
		}

		auto it = imageCache.find(filename);
		auto& val = it->second;
		return val.first;
	}
};

class ImageCacheTest
	: public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ImageCacheTest);
	CPPUNIT_TEST(refCountTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {}
	void tearDown() {}

	void refCountTest()
	{
		TestableImageCache imageCache;
		CPPUNIT_ASSERT_EQUAL(imageCache.count("foo"), std::size_t(0u));
		CPPUNIT_ASSERT_EQUAL(imageCache.count("bar"), std::size_t(0u));

		{
			auto image1{imageCache.getImage("foo")};
			(void)image1;
			CPPUNIT_ASSERT_EQUAL(imageCache.count("foo"), std::size_t(1u));
			CPPUNIT_ASSERT_EQUAL(imageCache.count("bar"), std::size_t(0u));

			auto image2 = imageCache.getImage("bar");
			CPPUNIT_ASSERT_EQUAL(imageCache.count("foo"), std::size_t(1u));
			CPPUNIT_ASSERT_EQUAL(imageCache.count("bar"), std::size_t(1u));

			auto image3 = imageCache.getImage("foo");
			CPPUNIT_ASSERT_EQUAL(imageCache.count("foo"), std::size_t(2u));
			CPPUNIT_ASSERT_EQUAL(imageCache.count("bar"), std::size_t(1u));

			{
				auto image4 = imageCache.getImage("foo");
				CPPUNIT_ASSERT_EQUAL(imageCache.count("foo"), std::size_t(3u));
				CPPUNIT_ASSERT_EQUAL(imageCache.count("bar"), std::size_t(1u));
			}

			CPPUNIT_ASSERT_EQUAL(imageCache.count("foo"), std::size_t(2u));
			CPPUNIT_ASSERT_EQUAL(imageCache.count("bar"), std::size_t(1u));
		}

		CPPUNIT_ASSERT_EQUAL(imageCache.count("foo"), std::size_t(0u));
		CPPUNIT_ASSERT_EQUAL(imageCache.count("bar"), std::size_t(0u));
	}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(ImageCacheTest);

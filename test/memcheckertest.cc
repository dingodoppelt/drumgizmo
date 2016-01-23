/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            memcheckertest.cc
 *
 *  Mon Jan 18 15:08:31 CET 2016
 *  Copyright 2016 André Nusser
 *  andre.nusser@googlemail.com
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

// This test should include:
// -------------------------
// * Load a small drumkit and check that the memchecker doesn't warn.
//	-> this assumes that the system is not already swapping which is sane imo.
// * Load a huge drumkit and check that the memchecker does warn.
// * Load a kit where we exactly know the size of the audio files and check if it's correct.
// * Check if the amount of free ram is in a realistic range.
//
// NOTE: This test will fail if your system is currently swapping or has an extremely small amount of RAM.

#include <cppunit/extensions/HelperMacros.h>

#include "../src/memchecker.h"
#include "../src/drumkit.h"
#include "../src/drumkitparser.h"

class test_memchecker
	: public CppUnit::TestFixture
	, public Memchecker
{
	CPPUNIT_TEST_SUITE(test_memchecker);
	CPPUNIT_TEST(small_drumkit);
	CPPUNIT_TEST(huge_drumkit);
	CPPUNIT_TEST(correct_size);
	CPPUNIT_TEST(check_free_ram);
	CPPUNIT_TEST_SUITE_END();
private:
	DrumKit kit;

	const std::string smallKitPath = "kit/small_kit.xml";
	const std::string hugeKitPath = "kit/huge_kit.xml";
	const std::string audiofile = "kit/ride-multi-channel.wav";
public:
	void setUp()
	{
		// just to be sure
		kit.clear();
	}

	void tearDown()
	{}

	void small_drumkit()
	{
		// load the small kit
		DrumKitParser parser(smallKitPath, kit);
		CPPUNIT_ASSERT(!parser.parse());

		// check if the memchecker thinks it fits into memory
		CPPUNIT_ASSERT(enoughFreeMemory(kit));
	}

	void huge_drumkit()
	{
		// load the huge kit
		DrumKitParser parser(hugeKitPath, kit);
		CPPUNIT_ASSERT(!parser.parse());

		// check if the memchecker thinks it doesn't fit into memory
		CPPUNIT_ASSERT(!enoughFreeMemory(kit));
	}

	void correct_size()
	{
		// check if the memchecker reports the right audiofile size
		CPPUNIT_ASSERT_EQUAL((size_t)2199332, calcBytesPerChannel(audiofile));

		// load the huge kit
		DrumKitParser parser(hugeKitPath, kit);
		CPPUNIT_ASSERT(!parser.parse());

		// check if the protected method of the memchecker reports the correct size
		CPPUNIT_ASSERT_EQUAL((size_t)71478290000, calcNeededMemory(kit));
	}

	void check_free_ram()
	{
		// check if the protected method reports a sane value of free ram
		size_t free_memory = calcFreeMemory();
		CPPUNIT_ASSERT(free_memory >= (size_t)1000 && free_memory <= (size_t)50000000000);
	}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(test_memchecker);

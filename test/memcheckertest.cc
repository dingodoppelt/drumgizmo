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

#include "drumkit_creator.h"

class MemCheckerTest
	: public CppUnit::TestFixture
	, public MemChecker
{
	CPPUNIT_TEST_SUITE(MemCheckerTest);
	CPPUNIT_TEST(small_drumkit);
	CPPUNIT_TEST(huge_drumkit);
	CPPUNIT_TEST(correct_size);
	CPPUNIT_TEST(check_free_ram);
	CPPUNIT_TEST_SUITE_END();
private:
	Settings settings;
	DrumKit kit;
	Random random;

	const std::string small_kit_path = drumkit_creator::createSmallKit("small_kit");
	const std::string huge_kit_path = drumkit_creator::createHugeKit("huge_kit");
	const std::string audiofile = drumkit_creator::createMultiChannelWav("multi_channel.wav");
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
		DrumKitParser parser(settings, kit, random);
		CPPUNIT_ASSERT(!parser.parseFile(small_kit_path));

		// check if the memchecker thinks it fits into memory
		CPPUNIT_ASSERT(enoughFreeMemory(kit));
	}

	void huge_drumkit()
	{
		// load the huge kit
		DrumKitParser parser(settings, kit, random);
		CPPUNIT_ASSERT(!parser.parseFile(huge_kit_path));

		// check if the memchecker thinks it doesn't fit into memory
		CPPUNIT_ASSERT(!enoughFreeMemory(kit));
	}

	void correct_size()
	{
		// check if the memchecker reports the right audiofile size
		uint64_t bytes_per_channel = 2199332;
		CPPUNIT_ASSERT_EQUAL(bytes_per_channel, calcBytesPerChannel(audiofile));

		// load the huge kit
		DrumKitParser parser(settings, kit, random);
		CPPUNIT_ASSERT(!parser.parseFile(huge_kit_path));

		// check if the protected method of the memchecker reports the correct size
		uint64_t needed_memory = 71478290000;
		CPPUNIT_ASSERT_EQUAL(needed_memory, calcNeededMemory(kit));
	}

	void check_free_ram()
	{
		// check if the protected method reports a sane value of free ram
		uint64_t free_memory = calcFreeMemory();
		uint64_t min_free_memory = 1000;
		uint64_t max_free_memory = 50000000000;
		CPPUNIT_ASSERT(free_memory >= min_free_memory && free_memory <= max_free_memory);
	}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(MemCheckerTest);

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audiocacheidmanagertest.cc
 *
 *  Thu Jan  7 15:42:31 CET 2016
 *  Copyright 2016 Bent Bisballe Nyeng
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
#include <cppunit/extensions/HelperMacros.h>

#include <audiocacheidmanager.h>

class TestableAudioCacheIDManager : public AudioCacheIDManager {
public:
	int getAvailableIds()
	{
		return availableids.size();
	}
};

class AudioCacheIDManagerTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(AudioCacheIDManagerTest);
	CPPUNIT_TEST(registerReleaseTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {}
	void tearDown() {}

	void registerReleaseTest()
	{
		TestableAudioCacheIDManager manager;
		manager.init(2);

		cache_t c1; c1.afile = (AudioCacheFile*)1;
		auto id1 = manager.registerId(c1);
		CPPUNIT_ASSERT(id1 != CACHE_DUMMYID);
		CPPUNIT_ASSERT(id1 != CACHE_NOID);
		CPPUNIT_ASSERT_EQUAL(1, manager.getAvailableIds());

		cache_t c2; c2.afile = (AudioCacheFile*)2;
		auto id2 = manager.registerId(c2);
		CPPUNIT_ASSERT(id2 != CACHE_DUMMYID);
		CPPUNIT_ASSERT(id2 != CACHE_NOID);
		CPPUNIT_ASSERT_EQUAL(0, manager.getAvailableIds());

		cache_t c3; c3.afile = (AudioCacheFile*)3;
		auto id3 = manager.registerId(c3);
		CPPUNIT_ASSERT(id3 == CACHE_DUMMYID);
		CPPUNIT_ASSERT_EQUAL(0, manager.getAvailableIds());

		cache_t& tc1 = manager.getCache(id1);
		CPPUNIT_ASSERT_EQUAL(c1.afile, tc1.afile);

		cache_t& tc2 = manager.getCache(id2);
		CPPUNIT_ASSERT_EQUAL(c2.afile, tc2.afile);

		manager.releaseId(id1);
		CPPUNIT_ASSERT_EQUAL(1, manager.getAvailableIds());

		cache_t c4; c4.afile = (AudioCacheFile*)4;
		auto id4 = manager.registerId(c4);
		CPPUNIT_ASSERT(id4 != CACHE_DUMMYID);
		CPPUNIT_ASSERT(id4 != CACHE_NOID);
		CPPUNIT_ASSERT_EQUAL(0, manager.getAvailableIds());

		cache_t& tc4 = manager.getCache(id4);
		CPPUNIT_ASSERT_EQUAL(c4.afile, tc4.afile);

		manager.releaseId(id2);
		CPPUNIT_ASSERT_EQUAL(1, manager.getAvailableIds());

		manager.releaseId(id4);
		CPPUNIT_ASSERT_EQUAL(2, manager.getAvailableIds());
	}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(AudioCacheIDManagerTest);
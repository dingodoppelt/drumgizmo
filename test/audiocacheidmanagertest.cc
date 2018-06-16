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
#include "dgunit.h"

#include <audiocacheidmanager.h>

class TestableAudioCacheIDManager
	: public AudioCacheIDManager
{
public:
	int getAvailableIDs()
	{
		return available_ids.size();
	}
};

class AudioCacheIDManagerTest
	: public DGUnit
{
public:
	AudioCacheIDManagerTest()
	{
		DGUNIT_TEST(AudioCacheIDManagerTest::registerReleaseTest);
	}

	void registerReleaseTest()
	{
		TestableAudioCacheIDManager manager;
		manager.init(2);

		cache_t c1; c1.afile = (AudioCacheFile*)1;
		auto id1 = manager.registerID(c1);
		DGUNIT_ASSERT(id1 != CACHE_DUMMYID);
		DGUNIT_ASSERT(id1 != CACHE_NOID);
		DGUNIT_ASSERT_EQUAL(1, manager.getAvailableIDs());

		cache_t c2; c2.afile = (AudioCacheFile*)2;
		auto id2 = manager.registerID(c2);
		DGUNIT_ASSERT(id2 != CACHE_DUMMYID);
		DGUNIT_ASSERT(id2 != CACHE_NOID);
		DGUNIT_ASSERT_EQUAL(0, manager.getAvailableIDs());

		cache_t c3; c3.afile = (AudioCacheFile*)3;
		auto id3 = manager.registerID(c3);
		DGUNIT_ASSERT(id3 == CACHE_DUMMYID);
		DGUNIT_ASSERT_EQUAL(0, manager.getAvailableIDs());

		cache_t& tc1 = manager.getCache(id1);
		DGUNIT_ASSERT_EQUAL(c1.afile, tc1.afile);

		cache_t& tc2 = manager.getCache(id2);
		DGUNIT_ASSERT_EQUAL(c2.afile, tc2.afile);

		manager.releaseID(id1);
		DGUNIT_ASSERT_EQUAL(1, manager.getAvailableIDs());

		cache_t c4; c4.afile = (AudioCacheFile*)4;
		auto id4 = manager.registerID(c4);
		DGUNIT_ASSERT(id4 != CACHE_DUMMYID);
		DGUNIT_ASSERT(id4 != CACHE_NOID);
		DGUNIT_ASSERT_EQUAL(0, manager.getAvailableIDs());

		cache_t& tc4 = manager.getCache(id4);
		DGUNIT_ASSERT_EQUAL(c4.afile, tc4.afile);

		manager.releaseID(id2);
		DGUNIT_ASSERT_EQUAL(1, manager.getAvailableIDs());

		manager.releaseID(id4);
		DGUNIT_ASSERT_EQUAL(2, manager.getAvailableIDs());
	}
};

// Registers the fixture into the 'registry'
static AudioCacheIDManagerTest test;

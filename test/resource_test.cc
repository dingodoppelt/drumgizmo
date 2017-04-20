/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            resource_test.cc
 *
 *  Fri Nov 13 18:50:52 CET 2015
 *  Copyright 2015 Bent Bisballe Nyeng
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

#include "../plugingui/resource.h"

#include "drumkit_creator.h"

class ResourceTester : public GUI::Resource {
public:
	ResourceTester(const std::string& name)
		: Resource(name)
	{}

	bool probeIsInternal()
	{
		return isInternal;
	}
};

class ResourceTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ResourceTest);
	CPPUNIT_TEST(externalReadTest);
	CPPUNIT_TEST(internalReadTest);
	CPPUNIT_TEST_SUITE_END();

	DrumkitCreator drumkit_creator;

public:
	void setUp() {}
	void tearDown() {}

	void externalReadTest()
	{
		auto filename = drumkit_creator.create0000Wav("0000.wav");

		ResourceTester rc(filename);
		CPPUNIT_ASSERT(!rc.probeIsInternal());
		CPPUNIT_ASSERT(rc.valid());
		CPPUNIT_ASSERT_EQUAL((size_t)46, rc.size());
	}

	void internalReadTest()
	{
		ResourceTester rc(":resources/bg.png");
		CPPUNIT_ASSERT(rc.probeIsInternal());
		CPPUNIT_ASSERT(rc.valid());
		CPPUNIT_ASSERT_EQUAL((size_t)1123, rc.size());
	}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(ResourceTest);

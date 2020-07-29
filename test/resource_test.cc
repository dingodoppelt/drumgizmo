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
#include "dgunit.h"

#include "../plugingui/resource.h"

#include "drumkit_creator.h"

class ResourceTester
	: public GUI::Resource
{
public:
	ResourceTester(const std::string& name)
		: GUI::Resource(name)
	{}

	bool probeIsInternal()
	{
		return isInternal;
	}
};

class ResourceTest
	: public DGUnit
{
public:
	ResourceTest()
	{
		DGUNIT_TEST(ResourceTest::externalReadTest);
		DGUNIT_TEST(ResourceTest::internalReadTest);
		DGUNIT_TEST(ResourceTest::failTest);
	}

	DrumkitCreator drumkit_creator;

	void externalReadTest()
	{
		auto filename = drumkit_creator.create0000Wav("0000.wav");

		ResourceTester rc(filename);
		DGUNIT_ASSERT(!rc.probeIsInternal());
		DGUNIT_ASSERT(rc.valid());
		DGUNIT_ASSERT_EQUAL((size_t)46, rc.size());
	}

	void internalReadTest()
	{
		ResourceTester rc(":resources/bg.png");
		DGUNIT_ASSERT(rc.probeIsInternal());
		DGUNIT_ASSERT(rc.valid());
		DGUNIT_ASSERT_EQUAL((size_t)1123, rc.size());
	}

	void failTest()
	{
		{
			ResourceTester rc("/tmp/");
			DGUNIT_ASSERT(!rc.valid());
		}

		{
			ResourceTester rc("no_such_file");
			DGUNIT_ASSERT(!rc.valid());
		}

		{
			ResourceTester rc(":no_such_file");
			DGUNIT_ASSERT(!rc.valid());
		}
	}
};

// Registers the fixture into the 'registry'
static ResourceTest test;

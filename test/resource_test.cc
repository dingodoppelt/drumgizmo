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
#include <uunit.h>

#include "../dggui/resource.h"

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
	: public uUnit
{
public:
	ResourceTest()
	{
		uUNIT_TEST(ResourceTest::externalReadTest);
		uUNIT_TEST(ResourceTest::internalReadTest);
		uUNIT_TEST(ResourceTest::failTest);
	}

	DrumkitCreator drumkit_creator;

	void externalReadTest()
	{
		auto filename = drumkit_creator.create0000Wav("0000.wav");

		ResourceTester rc(filename);
		uUNIT_ASSERT(!rc.probeIsInternal());
		uUNIT_ASSERT(rc.valid());
		uUNIT_ASSERT_EQUAL((size_t)46, rc.size());
	}

	void internalReadTest()
	{
		ResourceTester rc(":resources/bg.png");
		uUNIT_ASSERT(rc.probeIsInternal());
		uUNIT_ASSERT(rc.valid());
		uUNIT_ASSERT_EQUAL((size_t)1123, rc.size());
	}

	void failTest()
	{
		{
			ResourceTester rc("/tmp/");
			uUNIT_ASSERT(!rc.valid());
		}

		{
			ResourceTester rc("no_such_file");
			uUNIT_ASSERT(!rc.valid());
		}

		{
			ResourceTester rc(":no_such_file");
			uUNIT_ASSERT(!rc.valid());
		}
	}
};

// Registers the fixture into the 'registry'
static ResourceTest test;

/* -*- Mode: c++ -*- */
/***************************************************************************
 *            notifiertest.cc
 *
 *  Sat Jul 15 09:56:51 CEST 2017
 *  Copyright 2017 Bent Bisballe Nyeng
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

#include <vector>

#include <notifier.h>

class Probe
	: public Listener
{
public:
	Probe(std::vector<Probe*>& triggers)
		: triggers(triggers)
	{
	}

	void slot()
	{
		triggers.push_back(this);
	}

	std::vector<Probe*>& triggers;
};

class NotifierTest
	: public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(NotifierTest);
	CPPUNIT_TEST(testTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() { }
	void tearDown() { }

	//! This just creates some drumkit.
	void testTest()
	{
		Notifier<> notifier;
		std::vector<Probe*> triggers;
		Probe foo1(triggers);
		Probe foo2(triggers);

		{ // Order as initialisation
			notifier.connect(&foo1, &Probe::slot);
			notifier.connect(&foo2, &Probe::slot);
			notifier();
			std::vector<Probe*> ref;
			ref.push_back(&foo1);
			ref.push_back(&foo2);
			CPPUNIT_ASSERT_EQUAL(ref.size(), triggers.size());
			CPPUNIT_ASSERT_EQUAL(ref[0], triggers[0]);
			CPPUNIT_ASSERT_EQUAL(ref[1], triggers[1]);
			notifier.disconnect(&foo1);
			notifier.disconnect(&foo2);
			triggers.clear();
		}

		{ // Reverse order
			notifier.connect(&foo2, &Probe::slot);
			notifier.connect(&foo1, &Probe::slot);
			notifier();
			std::vector<Probe*> ref;
			ref.push_back(&foo2);
			ref.push_back(&foo1);
			CPPUNIT_ASSERT_EQUAL(ref.size(), triggers.size());
			CPPUNIT_ASSERT_EQUAL(ref[0], triggers[0]);
			CPPUNIT_ASSERT_EQUAL(ref[1], triggers[1]);
			notifier.disconnect(&foo1);
			notifier.disconnect(&foo2);
			triggers.clear();
		}

	}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(NotifierTest);

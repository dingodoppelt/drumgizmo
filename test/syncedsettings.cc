/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            syncedsettings.cc
 *
 *  Wed Mar 31 09:32:12 CET 2016
 *  Copyright 2016 Christian Glöckner
 *  cgloeckner@freenet.de
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

#include <syncedsettings.h>

class SyncedSettingsTest
	: public DGUnit
{
public:
	SyncedSettingsTest()
	{
		DGUNIT_TEST(SyncedSettingsTest::groupCanBeDefaultInitialized);
		DGUNIT_TEST(SyncedSettingsTest::groupDataCanBeCopied);

		DGUNIT_TEST(SyncedSettingsTest::accessorCanGetFields);
		DGUNIT_TEST(SyncedSettingsTest::accessorCanSetFields);

		DGUNIT_TEST(SyncedSettingsTest::groupHasCopyCtor);
		DGUNIT_TEST(SyncedSettingsTest::groupHasMoveCtor);
		DGUNIT_TEST(SyncedSettingsTest::groupHasCopyAssignOp);
		DGUNIT_TEST(SyncedSettingsTest::groupHasMoveAssignOp);

		DGUNIT_TEST(SyncedSettingsTest::mimicRealUse);
	}

private:
	struct TestData
	{
		float foo;
		bool bar;
		std::string msg;
	};

public:
	void groupCanBeDefaultInitialized()
	{
		Group<TestData> data;
	}

	void groupDataCanBeCopied()
	{
		Group<TestData> data;
		(TestData)data; // copies
	}

	void accessorCanSetFields()
	{
		Group<TestData> data;
		{
			Accessor<TestData> a{data};
			a.data.foo = 3.f;
			a.data.bar = false;
			a.data.msg = "hello";
		}
		TestData copy = data;
		DGUNIT_ASSERT_EQUAL(copy.foo, 3.f);
		DGUNIT_ASSERT_EQUAL(copy.bar, false);
		DGUNIT_ASSERT_EQUAL(copy.msg, std::string{"hello"});
	}

	void accessorCanGetFields()
	{
		Group<TestData> data;
		{
			Accessor<TestData> a{data};
			a.data.foo = 3.f;
			a.data.bar = false;
			a.data.msg = "hello";
		}
		// now read
		{
			Accessor<TestData> a{data};
			DGUNIT_ASSERT_EQUAL(a.data.foo, 3.f);
			DGUNIT_ASSERT_EQUAL(a.data.bar, false);
			DGUNIT_ASSERT_EQUAL(a.data.msg, std::string{"hello"});
		}
	}

	void groupHasCopyCtor()
	{
		Group<TestData> tmp;
		{
			Accessor<TestData> a{tmp};
			a.data.foo = 3.f;
			a.data.bar = false;
			a.data.msg = "hello";
		}
		Group<TestData> data{tmp};
		TestData copy = data;
		DGUNIT_ASSERT_EQUAL(copy.foo, 3.f);
		DGUNIT_ASSERT_EQUAL(copy.bar, false);
		DGUNIT_ASSERT_EQUAL(copy.msg, std::string{"hello"});
	}

	void groupHasMoveCtor()
	{
		Group<TestData> tmp;
		{
			Accessor<TestData> a{tmp};
			a.data.foo = 3.f;
			a.data.bar = false;
			a.data.msg = "hello";
		}
		Group<TestData> data{std::move(tmp)};
		TestData copy = data;
		DGUNIT_ASSERT_EQUAL(copy.foo, 3.f);
		DGUNIT_ASSERT_EQUAL(copy.bar, false);
		DGUNIT_ASSERT_EQUAL(copy.msg, std::string{"hello"});
	}

	void groupHasCopyAssignOp()
	{
		Group<TestData> tmp;
		{
			Accessor<TestData> a{tmp};
			a.data.foo = 3.f;
			a.data.bar = false;
			a.data.msg = "hello";
		}
		Group<TestData> data = tmp;
		TestData copy = data;
		DGUNIT_ASSERT_EQUAL(copy.foo, 3.f);
		DGUNIT_ASSERT_EQUAL(copy.bar, false);
		DGUNIT_ASSERT_EQUAL(copy.msg, std::string{"hello"});
	}

	void groupHasMoveAssignOp()
	{
		Group<TestData> tmp;
		{
			Accessor<TestData> a{tmp};
			a.data.foo = 3.f;
			a.data.bar = false;
			a.data.msg = "hello";
		}
		Group<TestData> data = std::move(tmp);
		TestData copy = data;
		DGUNIT_ASSERT_EQUAL(copy.foo, 3.f);
		DGUNIT_ASSERT_EQUAL(copy.bar, false);
		DGUNIT_ASSERT_EQUAL(copy.msg, std::string{"hello"});
	}

	void mimicRealUse()
	{
		struct Settings
		{
			struct Foo {
				float a{5};
				float b{3};
				bool enabled{true};
			};
			struct Bar {
				std::string label{"empty"};
				float bla{0.f};
			};

			Group<Foo> foo;
			Group<Bar> bar;
		};

		Settings s;

		// set bar settings
		{
			Accessor<Settings::Bar> tmp{s.bar};
			tmp.data.label = "hello world";
			tmp.data.bla = 3.14f;
		}

		// read foo settings
		{
			Accessor<Settings::Foo> tmp{s.foo};
			if (tmp.data.enabled) {
				// do some while locked
			}
		}
		// or:
		Settings::Foo copy = s.foo;
		if (copy.enabled) {
			// do some stuff without locking
		}
		DGUNIT_ASSERT(copy.enabled);
	}
};

// Registers the fixture into the 'registry'
static SyncedSettingsTest test;

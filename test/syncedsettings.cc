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
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
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

#include <syncedsettings.h>

class SyncedSettingsTest
	: public CppUnit::TestFixture {
		
	CPPUNIT_TEST_SUITE(SyncedSettingsTest);
	CPPUNIT_TEST(groupCanBeDefaultInitialized);
	CPPUNIT_TEST(groupDataCanBeCopied);
	CPPUNIT_TEST(groupCanBeCreatedByReference);
	CPPUNIT_TEST(groupCanBeCreatedByRvalueReference);
	
	CPPUNIT_TEST(accessorCanGetFields);
	CPPUNIT_TEST(accessorCanSetFields);
	
	CPPUNIT_TEST(groupHasCopyCtor);
	CPPUNIT_TEST(groupHasMoveCtor);
	CPPUNIT_TEST(groupHasCopyAssignOp);
	CPPUNIT_TEST(groupHasMoveAssignOp);
	CPPUNIT_TEST_SUITE_END();
	
	private:
		struct TestData {
			float foo;
			bool bar;
			std::string msg;
		};
	
	public:
		void setUp() {}
		void tearDown() {}
		
		void groupCanBeDefaultInitialized() {
			Group<TestData> data;
		}
		
		void groupDataCanBeCopied() {
			Group<TestData> data;
			(TestData)data; // copies
		}
		
		void groupCanBeCreatedByReference() {
			TestData tmp{3.f, false, "hello"};
			Group<TestData> data{tmp};
			TestData copy = data;
			CPPUNIT_ASSERT_EQUAL(copy.foo, 3.f);
			CPPUNIT_ASSERT_EQUAL(copy.bar, false);
			CPPUNIT_ASSERT_EQUAL(copy.msg, std::string{"hello"});
		}
		
		void groupCanBeCreatedByRvalueReference() {
			TestData tmp{3.f, false, "hello"};
			Group<TestData> data{std::move(tmp)};
			TestData copy = data;
			CPPUNIT_ASSERT_EQUAL(copy.foo, 3.f);
			CPPUNIT_ASSERT_EQUAL(copy.bar, false);
			CPPUNIT_ASSERT_EQUAL(copy.msg, std::string{"hello"});
		}
		
		void accessorCanGetFields() {
			TestData tmp{3.f, false, "hello"};
			Group<TestData> data{tmp};
			Accessor<TestData> a{data};
			CPPUNIT_ASSERT_EQUAL(a.data.foo, 3.f);
			CPPUNIT_ASSERT_EQUAL(a.data.bar, false);
			CPPUNIT_ASSERT_EQUAL(a.data.msg, std::string{"hello"});
		}
		
		void accessorCanSetFields() {
			Group<TestData> data;
			Accessor<TestData> a{data};
			a.data.foo = 3.f;
			a.data.bar = false;
			a.data.msg = "hello";
			TestData copy = data;
			CPPUNIT_ASSERT_EQUAL(copy.foo, 3.f);
			CPPUNIT_ASSERT_EQUAL(copy.bar, false);
			CPPUNIT_ASSERT_EQUAL(copy.msg, std::string{"hello"});
		}
	
		void groupHasCopyCtor() {
			Group<TestData> tmp;
			{
				Accessor<TestData> a{tmp};
				a.data.foo = 3.f;
				a.data.bar = false;
				a.data.msg = "hello";
			}
			Group<TestData> data{tmp};
			TestData copy = data;
			CPPUNIT_ASSERT_EQUAL(copy.foo, 3.f);
			CPPUNIT_ASSERT_EQUAL(copy.bar, false);
			CPPUNIT_ASSERT_EQUAL(copy.msg, std::string{"hello"});
		}
		
		void groupHasMoveCtor() {
			Group<TestData> tmp;
			{
				Accessor<TestData> a{tmp};
				a.data.foo = 3.f;
				a.data.bar = false;
				a.data.msg = "hello";
			}
			Group<TestData> data{std::move(tmp)};
			TestData copy = data;
			CPPUNIT_ASSERT_EQUAL(copy.foo, 3.f);
			CPPUNIT_ASSERT_EQUAL(copy.bar, false);
			CPPUNIT_ASSERT_EQUAL(copy.msg, std::string{"hello"});
		}
		
		void groupHasCopyAssignOp() {
			Group<TestData> tmp;
			{
				Accessor<TestData> a{tmp};
				a.data.foo = 3.f;
				a.data.bar = false;
				a.data.msg = "hello";
			}
			Group<TestData> data = tmp;
			TestData copy = data;
			CPPUNIT_ASSERT_EQUAL(copy.foo, 3.f);
			CPPUNIT_ASSERT_EQUAL(copy.bar, false);
			CPPUNIT_ASSERT_EQUAL(copy.msg, std::string{"hello"});
		}
		
		void groupHasMoveAssignOp() {
			Group<TestData> tmp;
			{
				Accessor<TestData> a{tmp};
				a.data.foo = 3.f;
				a.data.bar = false;
				a.data.msg = "hello";
			}
			Group<TestData> data = std::move(tmp);
			TestData copy = data;
			CPPUNIT_ASSERT_EQUAL(copy.foo, 3.f);
			CPPUNIT_ASSERT_EQUAL(copy.bar, false);
			CPPUNIT_ASSERT_EQUAL(copy.msg, std::string{"hello"});
		}
		
		// todo: further testing
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(SyncedSettingsTest);


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
	
	CPPUNIT_TEST(accessorCanGetFields);
	CPPUNIT_TEST(accessorCanSetFields);
	
	CPPUNIT_TEST(groupHasCopyCtor);
	CPPUNIT_TEST(groupHasMoveCtor);
	CPPUNIT_TEST(groupHasCopyAssignOp);
	CPPUNIT_TEST(groupHasMoveAssignOp);
	
	CPPUNIT_TEST(mimicRealUse);
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
		
		void accessorCanSetFields() {
			Group<TestData> data;
			{
				Accessor<TestData> a{data};
				a.data.foo = 3.f;
				a.data.bar = false;
				a.data.msg = "hello";
			}
			TestData copy = data;
			CPPUNIT_ASSERT_EQUAL(copy.foo, 3.f);
			CPPUNIT_ASSERT_EQUAL(copy.bar, false);
			CPPUNIT_ASSERT_EQUAL(copy.msg, std::string{"hello"});
		}
		
		void accessorCanGetFields() {
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
				CPPUNIT_ASSERT_EQUAL(a.data.foo, 3.f);
				CPPUNIT_ASSERT_EQUAL(a.data.bar, false);
				CPPUNIT_ASSERT_EQUAL(a.data.msg, std::string{"hello"});
			}
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
		
		void mimicRealUse() {
			struct Settings {
				struct Foo {
					int a, b;
					bool enabled;
				};
				struct Bar {
					float a, b;
					bool enabled;
				};
				struct Idk {
					std::string label;
					float bla;
				};
				
				Group<Foo> foo;
				Group<Bar> bar;
				Group<Idk> idk;
			};
			
			Settings s;
			
			// set some settings
			{
				Accessor<Settings::Foo> tmp{s.foo};
				tmp.data.enabled = true;
				tmp.data.a = 3;
			}
			{
				Accessor<Settings::Bar> tmp{s.bar};
				tmp.data.enabled = false;
				tmp.data.a = 0.f;
				tmp.data.b = 0.f;
			}
			{
				Accessor<Settings::Idk> tmp{s.idk};
				tmp.data.label = "hello world";
				tmp.data.bla = 3.14f;
			}
			
			// read some settings
			{
				Accessor<Settings::Foo> tmp{s.foo};
				if (tmp.data.enabled) {
					// do some while locked
				}
			}
			Settings::Bar copy = s.bar;
			if (copy.enabled) {
				// do some stuff without locking
			}
		}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(SyncedSettingsTest);


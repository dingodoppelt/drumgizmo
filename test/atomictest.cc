/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            atomic.cc
 *
 *  Wed Mar 23 09:17:12 CET 2016
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

#include <atomic.h>

class AtomicTest
	: public CppUnit::TestFixture {
		
	CPPUNIT_TEST_SUITE(AtomicTest);
	CPPUNIT_TEST(atomicIntUsesStandardImpl);
	CPPUNIT_TEST(atomicFloatUsesStandardImpl);
	CPPUNIT_TEST(atomicBoolUsesStandardImpl);
	CPPUNIT_TEST(atomicStringCanBeUsed);
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void setUp() {}
		void tearDown() {}
		
		void atomicIntUsesStandardImpl() {
			CPPUNIT_ASSERT(isUsingStandardImpl<int>());
		}
		
		void atomicFloatUsesStandardImpl() {
			CPPUNIT_ASSERT(isUsingStandardImpl<float>());
		}
		
		void atomicBoolUsesStandardImpl() {
			CPPUNIT_ASSERT(isUsingStandardImpl<bool>());
		}
		
		void atomicDoubleUsesStandardImpl() {
			CPPUNIT_ASSERT(isUsingStandardImpl<double>());
		}
		
		void atomicStringCanBeUsed() {
			// note: if it couldn't be used, the compiler would complain
			Atomic<std::string> tmp;
		}
		
		// todo: further testing
		
	private:
		template <typename T>
		bool isUsingStandardImpl() {
			return std::is_base_of<std::atomic<T>, Atomic<T>>::value;
		}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(AtomicTest);


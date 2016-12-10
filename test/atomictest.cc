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

#include <atomic.h>

class AtomicTest
	: public CppUnit::TestFixture {
		
	CPPUNIT_TEST_SUITE(AtomicTest);
	CPPUNIT_TEST(podAtomicsUseStandardImpl);
	CPPUNIT_TEST(nonPodAtomicsUseOwnImpl);
	CPPUNIT_TEST(podAtomicCanBeDefaultInitialized);
	CPPUNIT_TEST(nonPodAtomicCanBeDefaultInitialized);
	CPPUNIT_TEST(podAtomicCanBeValueInitialized);
	CPPUNIT_TEST(nonPodAtomicCanBeValueInitialized);
	CPPUNIT_TEST(podAtomicCanBeValueAssigned);
	CPPUNIT_TEST(nonPodAtomicCanBeValueAssigned);
	CPPUNIT_TEST(podAtomicsAreLockFree);
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void setUp() {}
		void tearDown() {}
		
		void podAtomicsUseStandardImpl() {
			CPPUNIT_ASSERT(isUsingStandardImpl<bool>());
			CPPUNIT_ASSERT(isUsingStandardImpl<unsigned short int>());
			CPPUNIT_ASSERT(isUsingStandardImpl<short int>());
			CPPUNIT_ASSERT(isUsingStandardImpl<unsigned int>());
			CPPUNIT_ASSERT(isUsingStandardImpl<int>());
			CPPUNIT_ASSERT(isUsingStandardImpl<unsigned long int>());
			CPPUNIT_ASSERT(isUsingStandardImpl<long int>());
			CPPUNIT_ASSERT(isUsingStandardImpl<unsigned long long int>());
			CPPUNIT_ASSERT(isUsingStandardImpl<long long int>());
			CPPUNIT_ASSERT(isUsingStandardImpl<float>());
			CPPUNIT_ASSERT(isUsingStandardImpl<double>());
			CPPUNIT_ASSERT(isUsingStandardImpl<long double>());
		}
		
		void nonPodAtomicsUseOwnImpl() {
			CPPUNIT_ASSERT(!isUsingStandardImpl<std::string>());
		}
		
		void podAtomicCanBeDefaultInitialized() {
			Atomic<int> i;
			// note: i is initialized with garbage
			(void)i; // prevent compiler 'unused' warning
		}
		
		void nonPodAtomicCanBeDefaultInitialized() {
			Atomic<std::string> s;
			CPPUNIT_ASSERT_EQUAL(s.load(), std::string{});
		}
		
		void podAtomicCanBeValueInitialized() {
			Atomic<int> i{5};
			CPPUNIT_ASSERT_EQUAL(i.load(), 5);
		}
		
		void nonPodAtomicCanBeValueInitialized() {
			Atomic<std::string> s{"hello world"};
			CPPUNIT_ASSERT_EQUAL(s.load(), std::string{"hello world"});
		}
		
		void podAtomicCanBeValueAssigned() {
			Atomic<int> i;
			i = 5;
			CPPUNIT_ASSERT_EQUAL(i.load(), 5);
		}
		
		void nonPodAtomicCanBeValueAssigned() {
			Atomic<std::string> s;
			s = "hello world";
			CPPUNIT_ASSERT_EQUAL(s.load(), std::string{"hello world"});
		}
		
		void podAtomicsAreLockFree() {
			CPPUNIT_ASSERT(isLockFree<bool>());
			CPPUNIT_ASSERT(isLockFree<unsigned short int>());
			CPPUNIT_ASSERT(isLockFree<short int>());
			CPPUNIT_ASSERT(isLockFree<unsigned int>());
			CPPUNIT_ASSERT(isLockFree<int>());
			CPPUNIT_ASSERT(isLockFree<unsigned long int>());
			CPPUNIT_ASSERT(isLockFree<long int>());
			CPPUNIT_ASSERT(isLockFree<unsigned long long int>());
			CPPUNIT_ASSERT(isLockFree<long long int>());
			CPPUNIT_ASSERT(isLockFree<float>());
			CPPUNIT_ASSERT(isLockFree<double>());
			
			// compile error: undefined reference to `__atomic_is_lock_free'
			//CPPUNIT_ASSERT(isLockFree<long double>());
		}
		
		// todo: further testing
		
	private:
		template <typename T>
		bool isUsingStandardImpl() {
			return std::is_base_of<std::atomic<T>, Atomic<T>>::value;
		}
		
		template <typename T>
		bool isLockFree() {
			Atomic<T> a;
			return a.is_lock_free();
		}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(AtomicTest);


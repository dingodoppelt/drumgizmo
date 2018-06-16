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
#include "dgunit.h"

#include <atomic.h>

class AtomicTest
	: public DGUnit
{
public:
	AtomicTest()
	{
		DGUNIT_TEST(AtomicTest::podAtomicsUseStandardImpl);
		DGUNIT_TEST(AtomicTest::nonPodAtomicsUseOwnImpl);
		DGUNIT_TEST(AtomicTest::podAtomicCanBeDefaultInitialized);
		DGUNIT_TEST(AtomicTest::nonPodAtomicCanBeDefaultInitialized);
		DGUNIT_TEST(AtomicTest::podAtomicCanBeValueInitialized);
		DGUNIT_TEST(AtomicTest::nonPodAtomicCanBeValueInitialized);
		DGUNIT_TEST(AtomicTest::podAtomicCanBeValueAssigned);
		DGUNIT_TEST(AtomicTest::nonPodAtomicCanBeValueAssigned);
		DGUNIT_TEST(AtomicTest::podAtomicsAreLockFree);
	}

	void podAtomicsUseStandardImpl()
	{
		DGUNIT_ASSERT(isUsingStandardImpl<bool>());
		DGUNIT_ASSERT(isUsingStandardImpl<unsigned short int>());
		DGUNIT_ASSERT(isUsingStandardImpl<short int>());
		DGUNIT_ASSERT(isUsingStandardImpl<unsigned int>());
		DGUNIT_ASSERT(isUsingStandardImpl<int>());
		DGUNIT_ASSERT(isUsingStandardImpl<unsigned long int>());
		DGUNIT_ASSERT(isUsingStandardImpl<long int>());
		DGUNIT_ASSERT(isUsingStandardImpl<unsigned long long int>());
		DGUNIT_ASSERT(isUsingStandardImpl<long long int>());
		DGUNIT_ASSERT(isUsingStandardImpl<float>());
		DGUNIT_ASSERT(isUsingStandardImpl<double>());
		DGUNIT_ASSERT(isUsingStandardImpl<long double>());
	}

	void nonPodAtomicsUseOwnImpl()
	{
		DGUNIT_ASSERT(!isUsingStandardImpl<std::string>());
	}

	void podAtomicCanBeDefaultInitialized()
	{
		Atomic<int> i;
		// note: i is initialized with garbage
		(void)i; // prevent compiler 'unused' warning
	}

	void nonPodAtomicCanBeDefaultInitialized()
	{
		Atomic<std::string> s;
		DGUNIT_ASSERT_EQUAL(s.load(), std::string{});
	}

	void podAtomicCanBeValueInitialized()
	{
		Atomic<int> i{5};
		DGUNIT_ASSERT_EQUAL(i.load(), 5);
	}

	void nonPodAtomicCanBeValueInitialized()
	{
		Atomic<std::string> s{"hello world"};
		DGUNIT_ASSERT_EQUAL(s.load(), std::string{"hello world"});
	}

	void podAtomicCanBeValueAssigned()
	{
		Atomic<int> i;
		i = 5;
		DGUNIT_ASSERT_EQUAL(i.load(), 5);
	}

	void nonPodAtomicCanBeValueAssigned()
	{
		Atomic<std::string> s;
		s = "hello world";
		DGUNIT_ASSERT_EQUAL(s.load(), std::string{"hello world"});
	}

	void podAtomicsAreLockFree()
	{
		DGUNIT_ASSERT(isLockFree<bool>());
		DGUNIT_ASSERT(isLockFree<unsigned short int>());
		DGUNIT_ASSERT(isLockFree<short int>());
		DGUNIT_ASSERT(isLockFree<unsigned int>());
		DGUNIT_ASSERT(isLockFree<int>());
		DGUNIT_ASSERT(isLockFree<unsigned long int>());
		DGUNIT_ASSERT(isLockFree<long int>());
		DGUNIT_ASSERT(isLockFree<float>());
		DGUNIT_ASSERT(isLockFree<std::size_t>());

		// NOTE: Not lock free on small systems
		//DGUNIT_ASSERT(isLockFree<unsigned long long int>());
		//DGUNIT_ASSERT(isLockFree<long long int>());
		//DGUNIT_ASSERT(isLockFree<double>());
		//DGUNIT_ASSERT(isLockFree<long double>());
	}

private:
	template <typename T>
	bool isUsingStandardImpl()
	{
		return std::is_base_of<std::atomic<T>, Atomic<T>>::value;
	}

	template <typename T>
	bool isLockFree()
	{
		Atomic<T> a;
		return a.is_lock_free();
	}
};

// Registers the fixture into the 'registry'
static AtomicTest test;

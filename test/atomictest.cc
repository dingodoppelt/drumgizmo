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
#include <uunit.h>

#include <atomic.h>

class AtomicTest
	: public uUnit
{
public:
	AtomicTest()
	{
		uUNIT_TEST(AtomicTest::podAtomicsUseStandardImpl);
		uUNIT_TEST(AtomicTest::nonPodAtomicsUseOwnImpl);
		uUNIT_TEST(AtomicTest::podAtomicCanBeDefaultInitialized);
		uUNIT_TEST(AtomicTest::nonPodAtomicCanBeDefaultInitialized);
		uUNIT_TEST(AtomicTest::podAtomicCanBeValueInitialized);
		uUNIT_TEST(AtomicTest::nonPodAtomicCanBeValueInitialized);
		uUNIT_TEST(AtomicTest::podAtomicCanBeValueAssigned);
		uUNIT_TEST(AtomicTest::nonPodAtomicCanBeValueAssigned);
		uUNIT_TEST(AtomicTest::podAtomicsAreLockFree);
	}

	void podAtomicsUseStandardImpl()
	{
		uUNIT_ASSERT(isUsingStandardImpl<bool>());
		uUNIT_ASSERT(isUsingStandardImpl<unsigned short int>());
		uUNIT_ASSERT(isUsingStandardImpl<short int>());
		uUNIT_ASSERT(isUsingStandardImpl<unsigned int>());
		uUNIT_ASSERT(isUsingStandardImpl<int>());
		uUNIT_ASSERT(isUsingStandardImpl<unsigned long int>());
		uUNIT_ASSERT(isUsingStandardImpl<long int>());
		uUNIT_ASSERT(isUsingStandardImpl<unsigned long long int>());
		uUNIT_ASSERT(isUsingStandardImpl<long long int>());
		uUNIT_ASSERT(isUsingStandardImpl<float>());
		uUNIT_ASSERT(isUsingStandardImpl<double>());
		uUNIT_ASSERT(isUsingStandardImpl<long double>());
	}

	void nonPodAtomicsUseOwnImpl()
	{
		uUNIT_ASSERT(!isUsingStandardImpl<std::string>());
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
		uUNIT_ASSERT_EQUAL(s.load(), std::string{});
	}

	void podAtomicCanBeValueInitialized()
	{
		Atomic<int> i{5};
		uUNIT_ASSERT_EQUAL(i.load(), 5);
	}

	void nonPodAtomicCanBeValueInitialized()
	{
		Atomic<std::string> s{"hello world"};
		uUNIT_ASSERT_EQUAL(s.load(), std::string{"hello world"});
	}

	void podAtomicCanBeValueAssigned()
	{
		Atomic<int> i;
		i = 5;
		uUNIT_ASSERT_EQUAL(i.load(), 5);
	}

	void nonPodAtomicCanBeValueAssigned()
	{
		Atomic<std::string> s;
		s = "hello world";
		uUNIT_ASSERT_EQUAL(s.load(), std::string{"hello world"});
	}

	void podAtomicsAreLockFree()
	{
		uUNIT_ASSERT(isLockFree<bool>());
		uUNIT_ASSERT(isLockFree<unsigned short int>());
		uUNIT_ASSERT(isLockFree<short int>());
		uUNIT_ASSERT(isLockFree<unsigned int>());
		uUNIT_ASSERT(isLockFree<int>());
		uUNIT_ASSERT(isLockFree<unsigned long int>());
		uUNIT_ASSERT(isLockFree<long int>());
		uUNIT_ASSERT(isLockFree<float>());
		uUNIT_ASSERT(isLockFree<std::size_t>());

		// NOTE: Not lock free on small systems
		//uUNIT_ASSERT(isLockFree<unsigned long long int>());
		//uUNIT_ASSERT(isLockFree<long long int>());
		//uUNIT_ASSERT(isLockFree<double>());
		//uUNIT_ASSERT(isLockFree<long double>());
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

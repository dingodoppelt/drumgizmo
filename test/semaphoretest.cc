/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            semaphoretest.cc
 *
 *  Tue Jun 14 22:04:24 CEST 2016
 *  Copyright 2016 Bent Bisballe Nyeng
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

#include <cassert>

#include <chrono>
#include <iostream>

#include "../src/semaphore.h"

std::chrono::nanoseconds dist(const std::chrono::duration<float>& a,
                              const std::chrono::duration<float>& b)
{
	if(a > b)
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(a - b);
	}

	return std::chrono::duration_cast<std::chrono::nanoseconds>(b - a);
}

class SemaphoreTest
	: public DGUnit
{
public:
	SemaphoreTest()
	{
		DGUNIT_TEST(SemaphoreTest::timeoutTest);
	}

public:
	void timeoutTest()
	{
		Semaphore sem(0);

		{ // 1000ms timeout
			auto start = std::chrono::steady_clock::now();
			bool res = sem.wait(std::chrono::milliseconds(1000));
			DGUNIT_ASSERT(!res); // false means timeout
			auto stop = std::chrono::steady_clock::now();

			// Allow +/-1ms skew
			DGUNIT_ASSERT(dist((stop - start), std::chrono::milliseconds(1000))
			               < std::chrono::milliseconds(60));
		}

		{ // 100ms timeout
			auto start = std::chrono::steady_clock::now();
			bool res = sem.wait(std::chrono::milliseconds(100));
			DGUNIT_ASSERT(!res); // false means timeout
			auto stop = std::chrono::steady_clock::now();

			// Allow +/-1ms skew
			DGUNIT_ASSERT(dist((stop - start), std::chrono::milliseconds(100))
			               < std::chrono::milliseconds(60));
		}
	}
};

// Registers the fixture into the 'registry'
static SemaphoreTest test;

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            randomtest.cc
 *
 *  Sat Mar 26 08:48:53 CET 2016
 *  Copyright 2016 Andr� Nusser
 *  andre.nusser@googlemail.com
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

#include <random.h>

#include <vector>
#include <cmath>

class RandomTest
	: public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(RandomTest);
	CPPUNIT_TEST(rangeTest);
	CPPUNIT_TEST(normalTest);
	CPPUNIT_TEST(chooseTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp()
	{
	}

	void tearDown()
	{
	
	}

	void rangeTest()
	{
		// check if random numbers are in the wanted range
		Random rand;

		float float_lb = -42.23;
		float float_ub = 666.666;
		int int_lb = -42;
		int int_ub = 23;
		for (int i = 0; i<10000; i++)
		{
			float rand_float = rand.floatInRange(float_lb, float_ub);
			float rand_int = rand.intInRange(int_lb, int_ub);
			CPPUNIT_ASSERT(rand_float >= float_lb && rand_float <= float_ub);
			CPPUNIT_ASSERT(rand_int >= int_lb && rand_int <= int_ub);
		}
		
		// check if the series of random numbers is the one we expect
		// for a certain seed.
		rand = Random(666);
		CPPUNIT_ASSERT_EQUAL(0, rand.intInRange(0,100));
		CPPUNIT_ASSERT_EQUAL(61, rand.intInRange(0,100));
		CPPUNIT_ASSERT_EQUAL(23, rand.intInRange(0,100));
	}

	void normalTest()
	{
		// This test might theoretically fail but it is extremely unlikely to happen.
		Random rand;

		float real_mean = 42.0;
		float real_stddev = 2.0;
		int nr_of_samples = 50000;

		float sum = 0.;
		float sum_of_squares = 0.;
		for (int i=0; i<nr_of_samples; i++)
		{
			float rand_float = rand.normalDistribution(real_mean, real_stddev);
			sum += rand_float;
			sum_of_squares += rand_float*rand_float;
		}

		float estimated_mean = sum/nr_of_samples;
		float estimated_stddev = sqrt(sum_of_squares/nr_of_samples - estimated_mean*estimated_mean);

		float epsilon = 0.1;
		CPPUNIT_ASSERT(estimated_mean >= real_mean-epsilon && estimated_mean <= real_mean+epsilon);
		CPPUNIT_ASSERT(estimated_stddev >= real_stddev-epsilon && estimated_stddev <= real_stddev+epsilon);
	}

	void chooseTest()
	{
		Random rand;

		std::vector<int> vec = { 42, 42, 42 };
		int nr_of_samples = 1000;

		for (int i=0; i<nr_of_samples; i++)
		{
			CPPUNIT_ASSERT_EQUAL(42, rand.choose(vec));
		}
	}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(RandomTest);
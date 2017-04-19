/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            random.cc
 *
 *  Wed Mar 23 19:17:24 CET 2016
 *  Copyright 2016 André Nusser
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
#include "random.h"

#include <chrono>
#include <type_traits>
#include <cmath>

Random::Random()
	: Random(std::chrono::system_clock::now().time_since_epoch().count())
{
}

Random::Random(unsigned int seed)
{
	setSeed(seed);
}

void Random::setSeed(unsigned int seed)
{
	generator.seed(seed);
}

int Random::intInRange(int lower_bound, int upper_bound)
{
	auto generate = [this]()
	{
		return (int)generator() - generator.min();
	};

	const int in_range = generator.max() - generator.min();
	const int out_range = upper_bound - lower_bound;

	int rand;

	// scale in_range DOWN to out_range.
	// (see: http://www.azillionmonkeys.com/qed/random.html)
	if (in_range > out_range)
	{
		const int rand_inv_range = in_range / (out_range + 1);

		do
		{
			rand = generate();
		}
		while (rand >= (out_range + 1) * rand_inv_range);

		rand = lower_bound + rand/rand_inv_range;
	}
	// scale in_range UP to out_range.
	// (see: http://stackoverflow.com/a/30738381)
	else if (in_range < out_range)
	{
		int scale = out_range / (in_range + 1);

		do
		{
			rand = generate() + intInRange(0, scale) * (in_range + 1);
		}
		while (rand < lower_bound && rand > upper_bound);

		rand = lower_bound + rand;
	}
	// naive case
	else
	{
		rand = lower_bound + generate();
	}

	return rand;
}

float Random::floatInRange(float lower_bound, float upper_bound)
{
	return generateFloat() * (upper_bound - lower_bound) + lower_bound;
}

// For details regarding the algorithm see:
// https://en.wikipedia.org/wiki/Marsaglia_polar_method
float Random::normalDistribution(float mean, float stddev)
{
	float u, v, s;
	do
	{
		u = 2.0*generateFloat() - 1.0;
		v = 2.0*generateFloat() - 1.0;
		s = (u * u) + (v * v);
	}
	while (s > 1.0 || s == 0.0);

	s = std::sqrt(-2*std::log(s) / s);

	return stddev * (v * s) + mean;
}

float Random::generateFloat()
{
	return std::generate_canonical<float,
	                               std::numeric_limits<float>::digits,
	                               decltype(generator)>(generator);
}

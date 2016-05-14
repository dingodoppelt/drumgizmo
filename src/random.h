/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            random.h
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
#pragma once

#include <random>
#include <vector>

class Random
{
public:
	Random();
	Random(unsigned int seed);

	void setSeed(unsigned int seed);

	//! \return random int in range [<lower_bound>, <upper_bound>].
	int intInRange(int lower_bound, int upper_bound);

	//! \return random float in range [<lower_bound>, <upper_bound>].
	float floatInRange(float lower_bound, float upper_bound);

	//! \return random float drawn from a normal distribution with mean <mean>
	//! and standard deviation <stddev>.
	float normalDistribution(float mean, float stddev);

	//! \return uniformly at random chosen element from <vec>.
	template <typename T>
	T& choose(std::vector<T>& vec);

private:
	std::default_random_engine generator;
};

template <typename T>
T& Random::choose(std::vector<T>& vec)
{
	std::uniform_int_distribution<size_t> distribution(0, vec.size()-1);
	size_t rand_index = distribution(generator);
	return vec[rand_index];
}

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

Random::Random()
	: Random(std::chrono::system_clock::now().time_since_epoch().count())
{

}

Random::Random(unsigned int seed)
{
	generator.seed(seed);
}

int Random::intInRange(int lower_bound, int upper_bound)
{
	std::uniform_int_distribution<int> distribution(lower_bound, upper_bound);
	return distribution(generator);
}

float Random::floatInRange(float lower_bound, float upper_bound)
{
	std::uniform_real_distribution<float> distribution(lower_bound, upper_bound);
	return distribution(generator);
}

float Random::normalDistribution(float mean, float stddev)
{
	std::normal_distribution<float> distribution(mean, stddev);
	return distribution(generator);
}

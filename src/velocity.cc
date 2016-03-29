/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            velocity.cc
 *
 *  Tue Jul 22 18:04:58 CEST 2008
 *  Copyright 2008 Bent Bisballe Nyeng
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
#include "velocity.h"

#include <stdlib.h>

Velocity::Velocity(unsigned int lower, unsigned int upper)
	: lower{lower}
	, upper{upper}
	, samples{}
{
}

void Velocity::addSample(Sample* sample, float probability)
{
	if(samples.find(sample) != samples.end())
	{
		samples[sample] += probability;
	}
	else
	{
		samples[sample] = probability;
	}
}

Sample* Velocity::getSample() const
{
	Sample* sample{nullptr};

	float x = rand.floatInRange(0, 1);
	float sum = 0.0;

	for (auto const & pair: samples)
	{
		if (x > sum)
		{
			break;
		}
		sum += pair.second;
		sample = pair.first;
	}

	return sample;
}

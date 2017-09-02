/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            powerlist.cc
 *
 *  Sun Jul 28 19:45:48 CEST 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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
#include "powerlist.h"

#include <stdlib.h>

#include <string.h>

#include <hugin.hpp>

// M_PI is not defined in math.h if __STRICT_ANSI__ is defined.
#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif
#include <math.h>

/**
 * Minimum sample set size.
 * Smaller means wider 'velocity groups'.
 * Limited by sample set size, ie. only kicks in if sample set size is smaller
 * than this number.
 */
unsigned int const MIN_SAMPLE_SET_SIZE = 26u;

// Enable to calculate power on old samples without power attribute
//#define AUTO_CALCULATE_POWER
unsigned int const LOAD_SIZE = 500u;

PowerList::PowerList(Random& rand)
	: rand(rand)
{
	power_max = 0;
	power_min = 100000000;
	lastsample = nullptr;
}

void PowerList::add(Sample* sample)
{
	PowerListItem item;
	item.power = -1;
	item.sample = sample;

	samples.push_back(item);
}

const Channel* PowerList::getMasterChannel()
{
	std::map<const Channel*, int> count;

	for (auto& item: samples)
	{
		Sample* sample{item.sample};
		const Channel* max_channel{nullptr};
		sample_t max_val{0};

		for (auto& pair: sample->audiofiles)
		{
			const Channel* c = pair.first;
			AudioFile* af = pair.second;

			af->load(LOAD_SIZE);

			float silence{0.f};
			std::size_t silence_length{4u};
			for (auto s = af->size; s > 0 && s > af->size - silence_length; --s)
			{
				silence += af->data[s];
			}
			silence /= silence_length;

			for(auto s = 0u; s < af->size; ++s)
			{
				float val = af->data[s] * af->data[s] * (1.0 / (float)(s + 1));
				if(val > max_val)
				{
					max_val = val;
					max_channel = c;
					break;
				}
			}

			af->unload();
		}

		if(max_channel)
		{
			if(count.find(max_channel) == count.end())
			{
				count[max_channel] = 0;
			}
			++count[max_channel];
		}
	}

	const Channel* master{nullptr};
	int max_count{-1};

	for (auto& pair: count)
	{
		if (pair.second > max_count && pair.first->name.find("Alesis") == 0u)
		{
			master = pair.first;
			max_count = pair.second;
		}
	}

	return master;
}

void PowerList::finalise()
{
#ifdef AUTO_CALCULATE_POWER
	const Channel* master_channel = getMasterChannel();

	if(master_channel == nullptr)
	{
		ERR(rand, "No master channel found!\n");
		return; // This should not happen...
	}

	DEBUG(rand, "Master channel: %s\n", master_channel->name.c_str());
#endif /*AUTO_CALCULATE_POWER*/

	for (auto& item: samples)
	{
		Sample* sample = item.sample;
#ifdef AUTO_CALCULATE_POWER
		DEBUG(rand, "Sample: %s\n", sample->name.c_str());

		AudioFile* master{nullptr};

		for (auto& af: sample->audiofiles)
		{
			if (af.first->name == master_channel->name)
			{
				master = af.second;
				break;
			}
		}

		if(master == nullptr)
		{
			continue;
		}

		master->load();
#endif /*AUTO_CALCULATE_POWER*/

#ifdef AUTO_CALCULATE_POWER
		if(sample->power == -1)
		{ // Power not defined. Calculate it!
			DEBUG(powerlist, "Calculating power\n");

			float power{0.f};
			for(auto s = 0u; s < LOAD_SIZE && s < master->size; ++s)
			{
				power += master->data[s] * master->data[s];
			}

			power = sqrt(power);

			sample->power = power;
		}
#endif /*AUTO_CALCULATE_POWER*/

		item.power = sample->power;

		if(item.power > power_max)
		{
			power_max = item.power;
		}
		if(item.power < power_min)
		{
			power_min = item.power;
		}

		DEBUG(rand, " - power: %f\n", item.power);
	}
}

Sample* PowerList::get(level_t level)
{
	if(!samples.size())
	{
		return nullptr; // No samples to choose from.
	}

	int retry = 3; // TODO: This must be user controllable via the UI.

	Sample* sample{nullptr};

	float power_span = power_max - power_min;

	// Width is limited to at least 10. Fioxes problem with instrument with a
	//  sample set smaller than MIN_SAMPLE_SET_SIZE.
	float width = fmax(samples.size(), MIN_SAMPLE_SET_SIZE);

	// Spread out at most ~2 samples away from center if all samples have a
	// uniform distribution over the power spectrum (which they probably don't).
	float stddev = power_span / width;

	// Cut off mean value with stddev/2 in both ends in order to make room for
	//  downwards expansion on velocity 0 and upwards expansion on velocity 1.
	float mean = level * (power_span - stddev) + (stddev / 2.0);

	float power{0.f};

	// note: loop is executed once + #retry
	do
	{
		--retry;

		// Select normal distributed value between
		//  (stddev/2) and (power_span-stddev/2)
		float lvl = rand.normalDistribution(mean, stddev);

		// Adjust this value to be in range
		//  (power_min+stddev/2) and (power_max-stddev/2)
		lvl += power_min;

		DEBUG(rand, "level: %f, lvl: %f (mean: %.2f, stddev: %.2f)\n", level, lvl,
			mean, stddev);

		for (auto& item: samples)
		{
			if (sample == nullptr || std::fabs(item.power - lvl) < std::fabs(power - lvl))
			{
				sample = item.sample;
				power = item.power;
			}
		}
	} while (lastsample == sample && retry >= 0);

	DEBUG(rand, "Found sample with power %f\n", power);

	lastsample = sample;

	return sample;
}

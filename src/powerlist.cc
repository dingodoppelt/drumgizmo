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

#include <algorithm>
#include <stdlib.h>
#include <string.h>

#include <hugin.hpp>

// M_PI is not defined in math.h if __STRICT_ANSI__ is defined.
#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif
#include <math.h>

#include "random.h"
#include "settings.h"

namespace
{

// Enable to calculate power on old samples without power attribute
//#define AUTO_CALCULATE_POWER
unsigned int const LOAD_SIZE = 500u;

} // end anonymous namespace

PowerList::PowerList()
{
	power_max = 0;
	power_min = 100000000;
}

void PowerList::add(Sample* sample)
{
	PowerListItem item;
	item.power = -1;
	item.sample = sample;

	samples.push_back(item);
}

// FIXME: remove all?
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

			af->load(nullptr, LOAD_SIZE);

			float silence{0.f};
			(void)silence;
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

// FIXME: clean up significantly!
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

	std::sort(samples.begin(), samples.end());
}

const PowerListItems& PowerList::getPowerListItems() const
{
	return samples;
}

float PowerList::getMaxPower() const
{
	return power_max;
}

float PowerList::getMinPower() const
{
	return power_min;
}

/* -*- Mode: c++ -*- */
/***************************************************************************
 *            sample_selection.h
 *
 *  Mon Mar  4 23:58:12 CET 2019
 *  Copyright 2019 André Nusser
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
#include "sample_selection.h"

#include <hugin.hpp>

#include "powerlist.h"
#include "random.h"
#include "settings.h"

#include <algorithm>

namespace
{

// Minimum sample set size.
// Smaller means wider 'velocity groups'.
// Limited by sample set size, ie. only kicks in if sample set size is smaller
// than this number.
std::size_t const MIN_SAMPLE_SET_SIZE = 26u;

float pow2(float f)
{
	return f*f;
}

} // end anonymous namespace

SampleSelection::SampleSelection(Settings& settings, Random& rand, const PowerList& powerlist)
	: settings(settings), rand(rand), powerlist(powerlist), alg(SelectionAlg::Objective)
{
}

void SampleSelection::setSelectionAlg(SelectionAlg alg)
{
	this->alg = alg;
}

void SampleSelection::finalise()
{
	last.assign(powerlist.getPowerListItems().size(), 0);
}

const Sample* SampleSelection::get(level_t level, std::size_t pos)
{
	// TODO: switch objective to default at some point
	switch (alg)
	{
		case SelectionAlg::Objective:
			return getObjective(level, pos);
			break;
		case SelectionAlg::Old:
		default:
			return getOld(level, pos);
	}
}

const Sample* SampleSelection::getOld(level_t level, std::size_t pos)
{
	auto velocity_stddev = settings.velocity_stddev.load();

	const auto& samples = powerlist.getPowerListItems();
	if(!samples.size())
	{
		return nullptr; // No samples to choose from.
	}

	int retry = settings.sample_selection_retry_count.load();

	Sample* sample{nullptr};

	auto power_max = powerlist.getMaxPower();
	auto power_min = powerlist.getMinPower();
	float power_span = power_max - power_min;

	// Width is limited to at least 10. Fixes problem with instrument with a
	//  sample set smaller than MIN_SAMPLE_SET_SIZE.
	float width = std::max(samples.size(), MIN_SAMPLE_SET_SIZE);

	// Spread out at most ~2 samples away from center if all samples have a
	// uniform distribution over the power spectrum (which they probably don't).
	float mean_stepwidth = power_span / width;

	// Cut off mean value with stddev/2 in both ends in order to make room for
	//  downwards expansion on velocity 0 and upwards expansion on velocity 1.
	float mean = level * (power_span - mean_stepwidth) + (mean_stepwidth / 2.0);
	float stddev = velocity_stddev * mean_stepwidth;

	std::size_t index{0};
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

		DEBUG(rand,
			  "level: %f, lvl: %f (mean: %.2f, stddev: %.2f, mean_stepwidth: %f, power_min: %f, power_max: %f)\n",
			  level, lvl, mean, stddev, mean_stepwidth, power_min, power_max);

		for (std::size_t i = 0; i < samples.size(); ++i)
		{
			auto const& item = samples[i];
			if (sample == nullptr || std::fabs(item.power - lvl) < std::fabs(power - lvl))
			{
				sample = item.sample;
				index = i;
				power = item.power;
			}
		}
	} while (lastsample == sample && retry >= 0);

	DEBUG(rand, "Chose sample with index: %d, power %f", (int)index, power);

	lastsample = sample;
	return sample;
}

const Sample* SampleSelection::getObjective(level_t level, std::size_t pos)
{
	const auto& samples = powerlist.getPowerListItems();
	if(!samples.size())
	{
		return nullptr; // No samples to choose from.
	}

	auto power_max = powerlist.getMaxPower();
	auto power_min = powerlist.getMinPower();
	float power_span = power_max - power_min;

	float mean = level - .5f/127.f; // XXX: this should actually be done when reading the events
	float stddev = settings.enable_velocity_modifier.load() ?
		settings.velocity_stddev.load()/127.0f : 0.;
	float lvl = power_min + rand.normalDistribution(mean, stddev)*power_span;

	std::size_t index_opt = 0;
	float power_opt{0.f};
	float value_opt{std::numeric_limits<float>::max()};
	// the following three values are mostly for debugging
	float random_opt = 0.;
	float distance_opt = 0.;
	float recent_opt = 0.;

	DEBUG(rand, "level: %f, lvl: %f (mean: %.2f, stddev: %.2f,"
		"power_min: %f, power_max: %f)\n", level, lvl, mean, stddev, power_min, power_max);

	const float f_distance = 2.0;
	const float f_recent = 1.0;
	const float f_random = .05;

	// start with most promising power value and then stop when reaching far values
	// which cannot become opt anymore
	auto closest_it = std::lower_bound(samples.begin(), samples.end(), lvl);
	std::size_t up_index = std::distance(samples.begin(), closest_it);
	std::size_t down_index = (up_index == 0 ? 0 : up_index - 1);
	float up_value_lb = (up_index < samples.size() ? f_distance*pow2(samples[up_index].power-lvl) : std::numeric_limits<float>::max());
	float down_value_lb = (up_index != 0 ? f_distance*pow2(samples[down_index].power-lvl) : std::numeric_limits<float>::max());

	std::size_t count = 0;
	do
	{
		std::size_t current_index;
		if (up_value_lb < down_value_lb)
		{
			current_index = up_index;
			if (up_index != samples.size()-1)
			{
				++up_index;
				up_value_lb = f_distance*pow2(samples[up_index].power-lvl);
			}
			else
			{
				up_value_lb = std::numeric_limits<float>::max();
			}
		}
		else
		{
			current_index = down_index;
			if (down_index != 0)
			{
				--down_index;
				down_value_lb = f_distance*pow2(samples[down_index].power-lvl);
			}
			else
			{
				down_value_lb = std::numeric_limits<float>::max();
			}
		}

		auto random = rand.floatInRange(0.,1.);
		auto distance = samples[current_index].power - lvl;
		auto recent = (float)settings.samplerate/std::max<std::size_t>(pos - last[current_index], 1);
		auto value = f_distance*pow2(distance) + f_recent*pow2(recent) + f_random*random;

		if (value < value_opt)
		{
			index_opt = current_index;
			power_opt = samples[current_index].power;
			value_opt = value;
			random_opt = random;
			distance_opt = distance;
			recent_opt = recent;
		}
		++count;
	}
	while (up_value_lb <= value_opt || down_value_lb <= value_opt);

	DEBUG(rand, "Chose sample with index: %d, value: %f, power %f, random: %f, distance: %f, recent: %f, count: %d", (int)index_opt, value_opt, power_opt, random_opt, distance_opt, recent_opt, (int)count);

	last[index_opt] = pos;
	return samples[index_opt].sample;
}

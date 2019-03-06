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
	: settings(settings), rand(rand), powerlist(powerlist)
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

const Sample* SampleSelection::getObjective(level_t level, std::size_t pos)
{
	auto velocity_stddev = settings.velocity_stddev.load();

	const auto& samples = powerlist.getPowerListItems();
	if(!samples.size())
	{
		return nullptr; // No samples to choose from.
	}

	auto power_max = powerlist.getMaxPower();
	auto power_min = powerlist.getMinPower();
	float power_span = power_max - power_min;

	// Width is limited to at least 10. Fixes problem with instrument with a
	// sample set smaller than MIN_SAMPLE_SET_SIZE.
	float width = std::max(samples.size(), MIN_SAMPLE_SET_SIZE);

	// Spread out at most ~2 samples away from center if all samples have a
	// uniform distribution over the power spectrum (which they probably don't).
	float mean_stepwidth = power_span / width;

	// Cut off mean value with stddev/2 in both ends in order to make room for
	// downwards expansion on velocity 0 and upwards expansion on velocity 1.
	float mean = level * (power_span - mean_stepwidth) + (mean_stepwidth / 2.0);
	float stddev = settings.enable_velocity_modifier.load() ? velocity_stddev * mean_stepwidth : 0.;

	std::size_t index_opt = 0;
	float power_opt{0.f};
	float value_opt{std::numeric_limits<float>::max()};
	// TODO: those are mostly for debugging at the moment
	float random_opt = 0.;
	float distance_opt = 0.;
	float recent_opt = 0.;

	// Select normal distributed value between
	// (stddev/2) and (power_span-stddev/2)
	float lvl = rand.normalDistribution(mean, stddev);

	// Adjust this value to be in range
	// (power_min+stddev/2) and (power_max-stddev/2)
	lvl += power_min;

	DEBUG(rand, "level: %f, lvl: %f (mean: %.2f, stddev: %.2f, mean_stepwidth: %f,"
		"power_min: %f, power_max: %f)\n", level, lvl, mean, stddev, mean_stepwidth,
		power_min, power_max);

	// TODO: expose parameters to GUI
	float alpha = 1.0;
	float beta = 1.0;
	float gamma = .5;

	// TODO: start with most promising power value and then stop when reaching far values
	// which cannot become opt anymore
	for (std::size_t i = 0; i < samples.size(); ++i)
	{
		auto const& item = samples[i];

		// compute objective function value
		auto random = rand.floatInRange(0.,1.);
		auto distance = item.power - lvl;
		auto recent = (float)settings.samplerate/std::max<std::size_t>(pos - last[i], 1);
		auto value = alpha*pow2(distance) + beta*pow2(recent) + gamma*random;

		if (value < value_opt)
		{
			index_opt = i;
			power_opt = item.power;
			value_opt = value;
			random_opt = random;
			distance_opt = distance;
			recent_opt = recent;
		}
	}

	DEBUG(rand, "Chose sample with index: %d, value: %f, power %f, random: %f, distance: %f, recent: %f", (int)index_opt, value_opt, power_opt, random_opt, distance_opt, recent_opt);

	last[index_opt] = pos;
	return samples[index_opt].sample;
}

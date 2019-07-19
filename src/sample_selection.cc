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

float pow2(float f)
{
	return f*f;
}

} // end anonymous namespace

SampleSelection::SampleSelection(Settings& settings, Random& rand, const PowerList& powerlist)
	: settings(settings), rand(rand), powerlist(powerlist)
{
}

void SampleSelection::finalise()
{
	last.assign(powerlist.getPowerListItems().size(), 0);
}

const Sample* SampleSelection::get(level_t level, std::size_t pos)
{
	const auto& samples = powerlist.getPowerListItems();
	if(!samples.size())
	{
		return nullptr; // No samples to choose from.
	}

	std::size_t index_opt = 0;
	float power_opt{0.f};
	float value_opt{std::numeric_limits<float>::max()};
	// the following three values are mostly for debugging
	float random_opt = 0.;
	float close_opt = 0.;
	float diverse_opt = 0.;

	const float f_close = settings.sample_selection_f_close.load();
	const float f_diverse = settings.sample_selection_f_diverse.load();
	const float f_random = settings.sample_selection_f_random.load();

	float const power_range = powerlist.getMaxPower() - powerlist.getMinPower();

	// start with most promising power value and then stop when reaching far values
	// which cannot become opt anymore
	auto closest_it = std::lower_bound(samples.begin(), samples.end(), level);
	std::size_t up_index = std::distance(samples.begin(), closest_it);
	std::size_t down_index = (up_index == 0 ? 0 : up_index - 1);

	auto const close_up = (samples[up_index].power-level)/power_range;
	auto const close_down = samples[down_index].power-level/power_range;
	float up_value_lb = (up_index < samples.size() ? f_close*pow2(close_up) : std::numeric_limits<float>::max());
	float down_value_lb = (up_index != 0 ? f_close*pow2(close_down) : std::numeric_limits<float>::max());

	std::size_t count = 0;
	do
	{
		DEBUG(rand, "%d %d", (int)up_index, (int)down_index);

		// at least avoid infinite loops in case of a bug...
		if (up_index == samples.size()-1 && down_index == 0) { break; }

		std::size_t current_index;
		if (up_value_lb < down_value_lb)
		{
			current_index = up_index;
			if (up_index != samples.size()-1)
			{
				++up_index;
				up_value_lb = f_close*pow2((samples[up_index].power-level)/power_range);
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
				down_value_lb = f_close*pow2((samples[down_index].power-level)/power_range);
			}
			else
			{
				down_value_lb = std::numeric_limits<float>::max();
			}
		}

		auto random = rand.floatInRange(0.,1.);
		auto close = (samples[current_index].power - level)/power_range;
		auto diverse = 1./(1. + (float)(pos - last[current_index])/settings.samplerate);
		auto value = f_close*pow2(close) + f_diverse*diverse + f_random*random;

		if (value < value_opt)
		{
			index_opt = current_index;
			power_opt = samples[current_index].power;
			value_opt = value;
			random_opt = random;
			close_opt = close;
			diverse_opt = diverse;
		}
		++count;
	}
	while (up_value_lb <= value_opt || down_value_lb <= value_opt);

	DEBUG(rand, "Chose sample with index: %d, value: %f, power %f, random: %f, close: %f, diverse: %f, count: %d", (int)index_opt, value_opt, power_opt, random_opt, close_opt, diverse_opt, (int)count);

	last[index_opt] = pos;
	return samples[index_opt].sample;

}

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            latencyfilter.cc
 *
 *  Fri Jun 10 22:42:53 CEST 2016
 *  Copyright 2016 Bent Bisballe Nyeng
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
#include "latencyfilter.h"

#include <cmath>
#include <hugin.hpp>

#include "settings.h"
#include "random.h"

LatencyFilter::LatencyFilter(Settings& settings, Random& random)
	: settings(settings)
	, random(random)
{
}

template<typename T1, typename T2>
static T1 getLatencySamples(T1 latency_ms, T2 samplerate)
{
	return latency_ms * samplerate / 1000.;
}

bool LatencyFilter::filter(event_t& event, std::size_t pos)
{
	auto enabled = settings.enable_latency_modifier.load();
	auto latency_ms = settings.latency_max_ms.load();
	auto samplerate = settings.samplerate.load();
	auto latency_laid_back_ms = settings.latency_laid_back_ms.load();
	auto latency_stddev = settings.latency_stddev.load();
	auto latency_regain = settings.latency_regain.load();

	if(!enabled)
	{
		return true;
	}

	auto latency = getLatencySamples(latency_ms, samplerate);
	auto latency_laid_back = getLatencySamples(latency_laid_back_ms, samplerate);

	// Assert latency_regain is within range [0; 1].
	assert(latency_regain >= 0.0f && latency_regain <= 1.0f);

	// User inputs 0 as no regain and 1 as instant - pow() is the other way around
	latency_regain *= -1.0f;
	latency_regain += 1.0f;

	float duration = (pos - latency_last_pos) / samplerate;
	latency_offset *= pow(latency_regain, duration);

	latency_last_pos = pos;

	float offset_min = latency * -1.0f;
	float offset_max = latency * 1.0f;

	float mean = 0.0f;//latency_laid_back;
	float stddev = latency_stddev;

	float offset = random.normalDistribution(mean, stddev);

	latency_offset += offset;

	if(latency_offset > offset_max) latency_offset = offset_max;
	if(latency_offset < offset_min) latency_offset = offset_min;

	DEBUG(offset, "latency: %d, offset: %f, drift: %f",
	      (int)latency, offset, latency_offset);

	event.offset += latency; // fixed latency offset
	event.offset += latency_laid_back; // laid back offset (user controlled)
	event.offset += latency_offset; // current drift

	settings.latency_current.store(latency_offset + latency_laid_back);

	return true;
}

std::size_t LatencyFilter::getLatency() const
{
	bool enabled = settings.enable_latency_modifier.load();
	if(enabled)
	{
		auto latency_ms = settings.latency_max_ms.load();
		auto samplerate = settings.samplerate.load();
		return getLatencySamples(latency_ms, samplerate);
	}

	return 0u;
}

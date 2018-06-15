/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            staminafilter.cc
 *
 *  Sat Jun 11 08:49:36 CEST 2016
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
#include "staminafilter.h"

#include "settings.h"

StaminaFilter::StaminaFilter(Settings& settings)
	: settings(settings)
{
}

bool StaminaFilter::filter(event_t& event, size_t pos)
{
	// Read out all values from settings.
	auto samplerate = settings.samplerate.load();
	auto velocity_modifier_falloff = settings.velocity_modifier_falloff.load();
	auto enable_velocity_modifier = settings.enable_velocity_modifier.load();
	auto velocity_modifier_weight = settings.velocity_modifier_weight.load();

	if(modifiers.find(event.instrument) == modifiers.end())
	{
		// On first lookup make sure we have sane values.
		auto& pair = modifiers[event.instrument];
		pair.first = 1.0f;
		pair.second = 0;
	}

	auto& pair = modifiers[event.instrument];
	auto& mod = pair.first;
	auto& lastpos = pair.second;

	if(enable_velocity_modifier == false)
	{
		mod = 1.0f;
		lastpos = 0;
	}

	if(enable_velocity_modifier)
	{
		mod += (pos - lastpos) / (samplerate * velocity_modifier_falloff);
		if(mod > 1.0f)
		{
			mod = 1.0f;
		}
	}

	event.velocity *= mod;

	if(enable_velocity_modifier)
	{
		lastpos = pos;
		mod *= velocity_modifier_weight;
	}

	settings.velocity_modifier_current.store(mod);

	return true;
}

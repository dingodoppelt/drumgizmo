/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            staminafilter.h
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
#pragma once

#include <cstddef>
#include <map>
#include <utility>

#include "inputfilter.h"

class Settings;

class StaminaFilter
	: public InputFilter
{
public:
	StaminaFilter(Settings& settings);

	bool filter(event_t& event, std::size_t pos) override;

	// Note getLatency not overloaded because this filter doesn't add latency.

private:
	Settings& settings;

	std::map<std::size_t, std::pair<float, std::size_t>> modifiers;
};

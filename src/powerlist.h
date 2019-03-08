/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            powerlist.h
 *
 *  Sun Jul 28 19:45:47 CEST 2013
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
#pragma once

#include <vector>

#include "sample.h"

struct PowerListItem
{
	Sample* sample;
	float power;

	bool operator<(const PowerListItem& other) const
	{
		return power < other.power;
	}
	bool operator<(level_t power) const
	{
		return this->power < power;
	}
};
using PowerListItems = std::vector<PowerListItem>;

class PowerList
{
public:
	PowerList();

	void add(Sample* s);
	void finalise(); ///< Call this when no more samples will be added.

	const PowerListItems& getPowerListItems() const;

	float getMaxPower() const;
	float getMinPower() const;

private:
	PowerListItems samples;
	float power_max;
	float power_min;

	const Channel* getMasterChannel();
};

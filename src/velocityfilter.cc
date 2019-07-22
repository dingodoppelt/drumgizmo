/* -*- Mode: c++ -*- */
/***************************************************************************
 *            velocityfilter.cc
 *
 *  Sun May 12 16:01:41 CEST 2019
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
#include "velocityfilter.h"

#include "random.h"
#include "settings.h"

VelocityFilter::VelocityFilter(Settings& settings, Random& random)
	: settings(settings), random(random)
{
}

bool VelocityFilter::filter(event_t& event, size_t pos)
{
	if (settings.enable_velocity_modifier.load())
	{
		float mean = event.velocity;
		float stddev = settings.velocity_stddev.load();
		// the 30.0f were determined empirically
		event.velocity = random.normalDistribution(mean, stddev / 30.0f);
	}

	return true;
}

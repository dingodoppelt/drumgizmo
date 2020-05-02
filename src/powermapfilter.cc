/* -*- Mode: c++ -*- */
/***************************************************************************
 *            powermapfilter.cc
 *
 *  Mon Apr 20 23:28:12 CEST 2020
 *  Copyright 2020 André Nusser
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
#include "powermapfilter.h"

#include "settings.h"

PowermapFilter::PowermapFilter(Settings& settings)
	: settings(settings)
{
}

bool PowermapFilter::filter(event_t& event, size_t pos)
{
	// the position is irrelevant for this filter
	(void) pos;

	settings.powermap_input.store(event.velocity);
	if (settings.enable_powermap.load())
	{
		powermap.setFixed0({settings.powermap_fixed0_x.load(), settings.powermap_fixed0_y.load()});
		powermap.setFixed1({settings.powermap_fixed1_x.load(), settings.powermap_fixed1_y.load()});
		powermap.setFixed2({settings.powermap_fixed2_x.load(), settings.powermap_fixed2_y.load()});
		powermap.setShelf(settings.powermap_shelf.load());

		event.velocity = powermap.map(event.velocity);
	}
	settings.powermap_output.store(event.velocity);

	return true;
}

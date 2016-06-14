/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            inputfilter.h
 *
 *  Tue Jun 14 20:42:52 CEST 2016
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

#include <event.h>

//! An abstract filter component for the InputProcessor class filter chain.
class InputFilter
{
public:
	//! Implement to filter a single event.
	//! \param[in,out] event The event being processed. The filter method
	//!  might alter any of the event parameters.
	//! \param pos The absolute position of the buffer in which the event in
	//!  event was received. Add event.pos to it to get the absolute position of
	//!  the event in the stream.
	//! \return true to keep the event, false to discard it.
	virtual bool filter(event_t& event, std::size_t pos) = 0;

	//! Returns the filter delay in samples.
	//! Overload if needed.
	virtual std::size_t getLatency() const
	{
		return 0;
	}
};

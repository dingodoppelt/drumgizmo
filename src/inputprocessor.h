/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            inputprocessor.h
 *
 *  Sat Apr 23 20:39:30 CEST 2016
 *  Copyright 2016 André Nusser
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
#pragma once

#include <vector>
#include <list>
#include <memory>

#include <event.h>

#include "drumkit.h"
#include "events.h"
#include "events_ds.h"
#include "id.h"
#include "inputfilter.h"

struct Settings;
class Random;

class InputProcessor
{
public:
	InputProcessor(Settings& settings,
	               DrumKit& kit,
	               EventsDS& events_ds,
	               Random& random);

	bool process(std::vector<event_t>& events,
	             std::size_t pos,
	             double resample_ratio);

	std::size_t getLatency() const;

private:
	DrumKit& kit;
	EventsDS& events_ds;
	bool is_stopping{false}; ///< Is set to true when a EventType::Stop event has been seen.

	bool processOnset(event_t& event, std::size_t pos, double resample_ratio);
	bool processChoke(event_t& event, std::size_t pos, double resample_ratio);
	bool processStop(event_t& event);

	//! Ramps down samples from events_ds is there are more groups playing than
	//! max_voices for a given instrument.
	void limitVoices(std::size_t instrument_id, std::size_t max_voices,
	                 float rampdown_time);

	std::vector<std::unique_ptr<InputFilter>> filters;

	Settings& settings;
	float original_velocity{0.0f};
};

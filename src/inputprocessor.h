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
#include "inputfilter.h"

struct Settings;
class Random;

class InputProcessor
{
public:
	InputProcessor(Settings& settings,
	               DrumKit& kit,
	               std::list<Event*>* activeevents,
	               Random& random);

	bool process(std::vector<event_t>& events,
	             std::size_t pos,
	             double resample_ratio);

	std::size_t getLatency() const;

private:
	DrumKit& kit;
	std::list<Event*>* activeevents;
	bool is_stopping; ///< Is set to true when a TYPE_STOP event has been seen.

	bool processOnset(event_t& event, std::size_t pos, double resample_ratio);
	bool processStop(event_t& event);

	std::vector<std::unique_ptr<InputFilter>> filters;
};

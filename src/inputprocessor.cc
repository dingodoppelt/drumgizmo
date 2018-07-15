/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            inputprocessor.cc
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
#include "inputprocessor.h"

#include <list>

#include <hugin.hpp>

#include "instrument.h"

#include "staminafilter.h"
#include "latencyfilter.h"

#include "cpp11fix.h"

InputProcessor::InputProcessor(Settings& settings,
                               DrumKit& kit,
                               std::list<Event*>* activeevents,
                               Random& random)
	: kit(kit)
	, activeevents(activeevents)
	, is_stopping(false)
	, settings(settings)
{
	// Build filter list
	filters.emplace_back(std::make_unique<StaminaFilter>(settings));
	filters.emplace_back(std::make_unique<LatencyFilter>(settings, random));
}

bool InputProcessor::process(std::vector<event_t>& events,
                             std::size_t pos,
                             double resample_ratio)
{
	for(auto& event: events)
	{
		if(event.type == TYPE_ONSET)
		{
			if(!processOnset(event, pos, resample_ratio))
			{
				continue;
			}
		}

		if(!processStop(event))
		{
			return false;
		}
	}

	return true;
}

std::size_t InputProcessor::getLatency() const
{
	std::size_t latency = 0;

	for(const auto& filter : filters)
	{
		latency += filter->getLatency();
	}

	return latency;
}

bool InputProcessor::processOnset(event_t& event,
                                  std::size_t pos,
                                  double resample_ratio)
{
	if(!kit.isValid())
	{
		return false;
	}

	std::size_t ev_instr = event.instrument;
	Instrument* instr = nullptr;

	if(ev_instr < kit.instruments.size())
	{
		instr = kit.instruments[ev_instr].get();
	}

	if(instr == nullptr || !instr->isValid())
	{
		ERR(inputprocessor, "Missing Instrument %d.\n", (int)ev_instr);
		return false;
	}

	if(instr->getGroup() != "")
	{
		// Add event to ramp down all existing events with the same groupname.
		for(Channel& ch: kit.channels)
		{
			for(Event* event: activeevents[ch.num])
			{
				if(event->getType() == Event::sample)
				{
					auto& event_sample = *static_cast<EventSample*>(event);
					if(event_sample.group == instr->getGroup() &&
					   event_sample.instrument != instr)
					{
						// Ramp down 14.7ms (3000 samples in 44k1Hz)
						event_sample.rampdown = settings.samplerate.load() / 14.7;
						// TODO: This must be configurable at some point...
						// ... perhaps even by instrument (ie. in the xml file)
						event_sample.ramp_start = event_sample.rampdown;
					}
				}
			}
		}
	}

	auto orig_level = event.velocity;
	for(auto& filter : filters)
	{
		// This line might change the 'event' variable
		bool keep = filter->filter(event, event.offset + pos);

		if(!keep)
		{
			return false; // Skip event completely
		}
	}

	Sample* sample = instr->sample(event.velocity, event.offset + pos);

	if(sample == nullptr)
	{
		ERR(inputprocessor, "Missing Sample.\n");
		return false;
	}

	auto selected_level =
		(sample->getPower() - instr->getMinPower()) /
		(instr->getMaxPower() - instr->getMinPower());
	settings.velocity_modifier_current.store(selected_level / orig_level);

	for(Channel& ch: kit.channels)
	{
		AudioFile* af = sample->getAudioFile(ch);
		if(af == nullptr || !af->isValid())
		{
			//DEBUG(inputprocessor, "Missing AudioFile.\n");
		}
		else
		{
			//DEBUG(inputprocessor, "Adding event %d.\n", event.offset);
			Event* evt = new EventSample(ch.num, 1.0, af, instr->getGroup(), instr);
			evt->offset = (event.offset + pos) * resample_ratio;
			activeevents[ch.num].push_back(evt);
		}
	}

	return true;
}

bool InputProcessor::processStop(event_t& event)
{
	if(event.type == TYPE_STOP)
	{
		is_stopping = true;
	}

	if(is_stopping)
	{
		// Count the number of active events.
		int num_active_events = 0;
		for(auto& ch: kit.channels)
		{
			num_active_events += activeevents[ch.num].size();
		}

		if(num_active_events == 0)
		{
			// No more active events - now we can stop the engine.
			return false;
		}
	}

	return true;
}

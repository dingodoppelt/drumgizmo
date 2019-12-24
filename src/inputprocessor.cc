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
#include "velocityfilter.h"

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
	filters.emplace_back(std::make_unique<VelocityFilter>(settings, random));
}

bool InputProcessor::process(std::vector<event_t>& events,
                             std::size_t pos,
                             double resample_ratio)
{
	for(auto& event: events)
	{
		if(event.type == EventType::OnSet)
		{
			if(!processOnset(event, pos, resample_ratio))
			{
				continue;
			}
		}

		if(event.type == EventType::Choke)
		{
			if(!processChoke(event, pos, resample_ratio))
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

	std::size_t instrument_id = event.instrument;
	Instrument* instr = nullptr;

	if(instrument_id < kit.instruments.size())
	{
		instr = kit.instruments[instrument_id].get();
	}

	if(instr == nullptr || !instr->isValid())
	{
		ERR(inputprocessor, "Missing Instrument %d.\n", (int)instrument_id);
		return false;
	}

	auto const original_level = event.velocity;
	for(auto& filter : filters)
	{
		// This line might change the 'event' variable
		bool keep = filter->filter(event, event.offset + pos);

		if(!keep)
		{
			return false; // Skip event completely
		}
	}

	if(instr->getGroup() != "")
	{
		// Add event to ramp down all existing events with the same groupname.
		for(const auto& ch : kit.channels)
		{
			for(auto active_event : activeevents[ch.num])
			{
				if(active_event->getType() == Event::sample)
				{
					auto& event_sample = *static_cast<EventSample*>(active_event);
					if(event_sample.group == instr->getGroup() &&
					   event_sample.instrument_id != instrument_id &&
					   event_sample.rampdown_count == -1) // Only if not already ramping.
					{
						// Fixed group rampdown time of 68ms, independent of samplerate
						std::size_t ramp_length = (68./1000.)*settings.samplerate.load();
						event_sample.rampdown_count = ramp_length;
						event_sample.rampdown_offset = event.offset;
						event_sample.ramp_length = ramp_length;
					}
				}
			}
		}
	}

	for(const auto& choke : instr->getChokes())
	{
		// Add event to ramp down all existing events with the same groupname.
		for(const auto& ch : kit.channels)
		{
			for(auto active_event : activeevents[ch.num])
			{
				if(active_event->getType() == Event::sample)
				{
					auto& event_sample = *static_cast<EventSample*>(active_event);
					if(choke.instrument_id == event_sample.instrument_id &&
					   event_sample.rampdown_count == -1) // Only if not already ramping.
					{
						// Choketime is in ms
						std::size_t ramp_length =
							(choke.choketime / 1000.0) * settings.samplerate.load();
						event_sample.rampdown_count = ramp_length;
						event_sample.rampdown_offset = event.offset;
						event_sample.ramp_length = ramp_length;
					}
				}
			}
		}

	}

	auto const power_max = instr->getMaxPower();
	auto const power_min = instr->getMinPower();
	float const power_span = power_max - power_min;
	float const instrument_level = power_min + event.velocity*power_span;
	const auto sample = instr->sample(instrument_level, event.offset + pos);

	if(sample == nullptr)
	{
		ERR(inputprocessor, "Missing Sample.\n");
		return false;
	}

	auto const selected_level = (sample->getPower() - power_min)/power_span;
	settings.velocity_modifier_current.store(selected_level/original_level);

	for(Channel& ch: kit.channels)
	{
		const auto af = sample->getAudioFile(ch);
		if(af == nullptr || !af->isValid())
		{
			//DEBUG(inputprocessor, "Missing AudioFile.\n");
		}
		else
		{
			//DEBUG(inputprocessor, "Adding event %d.\n", event.offset);
			auto evt = new EventSample(ch.num, 1.0, af, instr->getGroup(),
			                           instrument_id);
			evt->offset = (event.offset + pos) * resample_ratio;
			if(settings.normalized_samples.load() && sample->getNormalized())
			{
				evt->scale *= event.velocity;
			}

			activeevents[ch.num].push_back(evt);
		}
	}

	return true;
}

bool InputProcessor::processChoke(event_t& event,
                                  std::size_t pos,
                                  double resample_ratio)
{
	if(!kit.isValid())
	{
		return false;
	}

	std::size_t instrument_id = event.instrument;
	Instrument* instr = nullptr;

	if(instrument_id < kit.instruments.size())
	{
		instr = kit.instruments[instrument_id].get();
	}

	if(instr == nullptr || !instr->isValid())
	{
		ERR(inputprocessor, "Missing Instrument %d.\n", (int)instrument_id);
		return false;
	}

	for(auto& filter : filters)
	{
		// This line might change the 'event' variable
		bool keep = filter->filter(event, event.offset + pos);

		if(!keep)
		{
			return false; // Skip event completely
		}
	}

	// Add event to ramp down all existing events with the same groupname.
	for(const auto& ch : kit.channels)
	{
		for(auto active_event : activeevents[ch.num])
		{
			if(active_event->getType() == Event::sample)
			{
				auto& event_sample = *static_cast<EventSample*>(active_event);
				if(event_sample.instrument_id == instrument_id &&
				   event_sample.rampdown_count == -1) // Only if not already ramping.
				{
					// Fixed group rampdown time of 68ms, independent of samplerate
					std::size_t ramp_length = (68./1000.)*settings.samplerate.load();
					event_sample.rampdown_count = ramp_length;
					event_sample.rampdown_offset = event.offset;
					event_sample.ramp_length = ramp_length;
				}
			}
		}
	}

	return true;
}

bool InputProcessor::processStop(event_t& event)
{
	if(event.type == EventType::Stop)
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

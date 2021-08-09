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

#include "latencyfilter.h"
#include "powermapfilter.h"
#include "staminafilter.h"
#include "velocityfilter.h"

#include "cpp11fix.h"

class VelocityStorer
	: public InputFilter
{
public:
	VelocityStorer(float& original_velocity)
		: original_velocity(original_velocity)
	{
	}

	bool filter(event_t& event, std::size_t pos) override
	{
		original_velocity = event.velocity;
		return true;
	}

private:
	float& original_velocity;
};

class Reporter
	: public InputFilter
{
public:
	Reporter(Settings& settings, float& original_velocity)
		: settings(settings)
		, original_velocity(original_velocity)
	{
	}

	bool filter(event_t& event, std::size_t pos) override
	{
		settings.velocity_modifier_current.store(event.velocity / original_velocity);
		return true;
	}

private:
	Settings& settings;
	float& original_velocity;
};

InputProcessor::InputProcessor(Settings& settings,
                               DrumKit& kit,
                               EventsDS& events_ds,
                               Random& random)
	: kit(kit)
	, events_ds(events_ds)
	, settings(settings)
{
	// Build filter list
	filters.emplace_back(std::make_unique<PowermapFilter>(settings));
	filters.emplace_back(std::make_unique<VelocityStorer>(original_velocity));
	filters.emplace_back(std::make_unique<StaminaFilter>(settings));
	filters.emplace_back(std::make_unique<LatencyFilter>(settings, random));
	filters.emplace_back(std::make_unique<VelocityFilter>(settings, random));
	filters.emplace_back(std::make_unique<Reporter>(settings, original_velocity));
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

//! Applies choke with rampdown time in ms to event starting at offset.
static void applyChoke(Settings& settings, SampleEvent& event,
                       double length_ms, timepos_t offset)
{
	std::size_t ramp_length = (length_ms / 1000.) * settings.samplerate.load();
	event.rampdown_count = ramp_length;
	event.rampdown_offset = offset;
	event.ramp_length = ramp_length;
}

//! Applies choke group actions to active events based on the input event
static void applyChokeGroup(Settings& settings, DrumKit& kit,
                            Instrument& instr, event_t& event,
                            EventsDS& events_ds)
{
	std::size_t instrument_id = event.instrument;
	if(instr.getGroup() == "")
	{
		return;
	}

	// Add event to ramp down all existing events with the same groupname.
	for(const auto& ch : kit.channels)
	{
		if(ch.num >= NUM_CHANNELS) // kit may have more channels than the engine
		{
			continue;
		}

		for(auto& event_sample : events_ds.iterateOver<SampleEvent>(ch.num))
		{
			if(event_sample.group == instr.getGroup() &&
			   event_sample.instrument_id != instrument_id &&
			   event_sample.rampdown_count == -1) // Only if not already ramping.
			{
				// Fixed group rampdown time of 68ms, independent of samplerate
				applyChoke(settings, event_sample, 68, event.offset);
			}
		}
	}
}

//! Applies directed choke actions to active events based on the input event
static void applyDirectedChoke(Settings& settings, DrumKit& kit,
                               Instrument& instr, event_t& event,
                               EventsDS& events_ds)
{
	for(const auto& choke : instr.getChokes())
	{
		// Add event to ramp down all existing events with the same groupname.
		for(const auto& ch : kit.channels)
		{
			if(ch.num >= NUM_CHANNELS) // kit may have more channels than the engine
			{
				continue;
			}

			for(auto& event_sample : events_ds.iterateOver<SampleEvent>(ch.num))
			{
				if(choke.instrument_id == event_sample.instrument_id &&
				   event_sample.rampdown_count == -1) // Only if not already ramping.
				{
					// choke.choketime is in ms
					applyChoke(settings, event_sample, choke.choketime, event.offset);
				}
			}
		}

	}
}

bool InputProcessor::processOnset(event_t& event, std::size_t pos,
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

	original_velocity = event.velocity;
	for(auto& filter : filters)
	{
		// This line might change the 'event' variable
		bool keep = filter->filter(event, event.offset + pos);

		if(!keep)
		{
			return false; // Skip event completely
		}
	}

	// Mute other instruments in the same instrument/choke group
	applyChokeGroup(settings, kit, *instr, event, events_ds);

	// Apply directed chokes to mute other instruments if needed
	applyDirectedChoke(settings, kit, *instr, event, events_ds);

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

	if(settings.enable_voice_limit.load())
	{
		limitVoices(instrument_id,
		            settings.voice_limit_max.load(),
		            settings.voice_limit_rampdown.load());
	}

	//Given that audio files could be invalid, maybe we must add the new
	//group just before adding the first new sample...
	bool new_group_added = false;
	for(Channel& ch: kit.channels)
	{
		if(ch.num >= NUM_CHANNELS) // kit may have more channels than the engine
		{
			continue;
		}

		const auto af = sample->getAudioFile(ch);
		if(af == nullptr || !af->isValid())
		{
			//DEBUG(inputprocessor, "Missing AudioFile.\n");
		}
		else
		{
			//DEBUG(inputprocessor, "Adding event %d.\n", event.offset);
			if(!new_group_added)
			{
				new_group_added=true;
				events_ds.startAddingNewGroup(instrument_id);
			}

			auto& event_sample =
				events_ds.emplace<SampleEvent>(ch.num, ch.num, 1.0, af,
				                               instr->getGroup(), instrument_id);

			event_sample.offset = (event.offset + pos) * resample_ratio;
			if(settings.normalized_samples.load() && sample->getNormalized())
			{
				event_sample.scale *= event.velocity;
			}
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
		if(ch.num >= NUM_CHANNELS) // kit may have more channels than the engine
		{
			continue;
		}

		for(auto& event_sample : events_ds.iterateOver<SampleEvent>(ch.num))
		{
			if(event_sample.instrument_id == instrument_id &&
			   event_sample.rampdown_count == -1) // Only if not already ramping.
			{
				// Fixed group rampdown time of 68ms, independent of samplerate
				applyChoke(settings, event_sample, 450, event.offset);
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
			if(ch.num >= NUM_CHANNELS) // kit may have more channels than the engine
			{
				continue;
			}

			num_active_events += events_ds.numberOfEvents(ch.num);
		}

		if(num_active_events == 0)
		{
			// No more active events - now we can stop the engine.
			return false;
		}
	}

	return true;
}

void InputProcessor::limitVoices(std::size_t instrument_id,
                                 std::size_t max_voices,
                                 float rampdown_time)
{
	const auto& group_ids=events_ds.getSampleEventGroupIDsOf(instrument_id);

	if(group_ids.size() <= max_voices)
	{
		return;
	}

	//Filter out ramping events...
	auto filter_ramping_predicate =
		[this](EventGroupID group_id) -> bool
		{
			const auto& event_ids=events_ds.getEventIDsOf(group_id);
			//TODO: This should not happen.
			if(!event_ids.size())
			{
				return false;
			}

			const auto&	sample=events_ds.get<SampleEvent>(event_ids[0]);
			return !sample.rampdownInProgress();
		};

	EventGroupIDs non_ramping;
	std::copy_if(std::begin(group_ids),
	             std::end(group_ids),
	             std::back_inserter(non_ramping), filter_ramping_predicate);

	if(!non_ramping.size())
	{
		return;
	}

	//Let us get the eldest...
	//TODO: where is the playhead? Should we add it to the offset?
	auto compare_event_offsets =
		[this](EventGroupID a, EventGroupID b)
		{
			const auto& event_ids_a=events_ds.getEventIDsOf(a);
			const auto& event_ids_b=events_ds.getEventIDsOf(b);

			const auto&	sample_a=events_ds.get<SampleEvent>(event_ids_a[0]);
			const auto& sample_b=events_ds.get<SampleEvent>(event_ids_b[0]);
			return sample_a.offset < sample_b.offset;
		};

	auto it = std::min_element(std::begin(non_ramping),
	                           std::end(non_ramping),
	                           compare_event_offsets);
	if(it == std::end(non_ramping))
	{
		return;
	}

	const auto& event_ids = events_ds.getEventIDsOf(*it);
	for(const auto& event_id : event_ids)
	{
		auto& sample=events_ds.get<SampleEvent>(event_id);
		applyChoke(settings, sample, rampdown_time, sample.offset);
	}
}

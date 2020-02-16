/* -*- Mode: c++ -*- */
/***************************************************************************
 *            events_ds.h
 *
 *  Sun Jan 12 00:17:31 CET 2020
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
#pragma once

#include <config.h>

#include <algorithm>
#include <array>
#include <vector>

#include "events.h"
#include "id.h"
#include "memory_heap.h"
#include "range.h"
#include "instrument.h"

//! TODO: document class as a whole
//! TODO: What s does DS stand for?
// TODO: make it possible to choose sizes
class EventsDS
{
public:
	EventsDS() = default; // TODO: Is this needed?

	//! TODO: document all the (public) things!
	template <typename T, typename... Args>
	T& emplace(int ch, Args&&... args);

	//! TODO: document all the (public) things!
	void remove(EventID event_id);

	//! TODO: document all the (public) things!
	std::size_t numberOfEvents(int ch) const;

	//! TODO: document all the (public) things!
	template <typename T>
	ContainerRange<std::vector<T>> iterateOver(int ch);

	//! TODO: document all the (public) things!
	const EventGroupIDs& getSampleEventGroupIDsOf(InstrumentID instrument_id) const;

	//! TODO: document all the (public) things!
	const EventIDs& getEventIDsOf(EventGroupID event_group_id) const;

	//! TODO: document all the (public) things!
	void startAddingNewGroup(InstrumentID instrument_id = InstrumentID());

private:
	struct ChannelData
	{
		std::vector<SampleEvent> sample_events;
	};

	using ChannelEventIndex = std::size_t;

	struct EventInfo
	{
		Event::Type type;
		int ch;
		ChannelEventIndex channel_event_index;

		EventInfo(Event::Type type, int ch, ChannelEventIndex channel_event_index)
			: type(type), ch(ch), channel_event_index(channel_event_index) {}
	};

	struct GroupData
	{
		EventIDs event_ids;
		Event::Type type;

		// SampleEvent specific data
		std::size_t instrument_index;
	};

	// general data
	std::array<ChannelData, NUM_CHANNELS> channel_data_array;
	MemoryHeap<EventInfo> id_to_info;
	MemoryHeap<GroupData> id_to_group_data;

	// SampleEvent specific data
	// TODO: maybe use something that actually has the size of the number of instruments
	// Also, can we guarantee that there are at most 128 instrument ids?
	std::array<EventGroupIDs, 128> instruments_sample_event_group_ids;

	EventGroupID current_group_id;
	InstrumentID current_groups_instrument_id;

	void removeGroup(EventGroupID group_id, InstrumentID instrument_id = InstrumentID());
};

template <typename T, typename... Args>
T& EventsDS::emplace(int ch, Args&&... args)
{
	if (std::is_same<T, SampleEvent>::value)
	{
		auto& sample_events = channel_data_array[ch].sample_events;
		auto channel_event_index = sample_events.size();
		sample_events.emplace_back(std::forward<Args>(args)...);

		auto event_id = id_to_info.emplace(Event::Type::SampleEvent, ch, channel_event_index);
		id_to_group_data.get(current_group_id).event_ids.push_back(event_id);

		auto& sample_event = sample_events.back();
		sample_event.id = event_id;
		sample_event.group_id = current_group_id;
		assert(sample_event.instrument_id == current_groups_instrument_id);

		return sample_event;
	}

	assert(false); // TODO: This should probably be a static_assert instead?
}

template <typename T>
ContainerRange<std::vector<T>> EventsDS::iterateOver(int ch)
{
	if (std::is_same<T, SampleEvent>::value)
	{
		auto& sample_events = channel_data_array[ch].sample_events;
		return ContainerRange<std::vector<T>>(sample_events.begin(), sample_events.end());
	}
}

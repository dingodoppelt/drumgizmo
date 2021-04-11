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

//! EventsDS (events data structure) is the central engine events data structure.
//! It enables the following fast operations:
//! * Add a new event
//! * Remove an event
//! * Iterate over all events of a channel
//! * Get all events of a certain group
//! * ...
//!
//! All operations are constant time (assuming bounded group sizes). Also, it
//! assignes the event ids and group ids. Certain functions may be specific to certain event types.
//!
//! This data structure makes several assumptions about its usage:
//! * we assume that we add a group in one batch without deleting anything in-between
//! * we assume that an event_id isn't used after remove(event_id) was called
//! * we assume that before starting to emplace a new group, startAddingNewGroup is called

// TODO: make it possible to choose sizes of all the containers on initialization
// such that we avoid allocations all together as long as we stay in these bounds.

class EventsDS
{
public:
	//! Adds a new event of type T (via emplace syntax) to the data structure.
	template <typename T, typename... Args>
	T& emplace(channel_t ch, Args&&... args);

	//! Removes the event with id being event_id.
	void remove(EventID event_id);

	//! Returns a reference to the element with id begin event_id. Note that to
	//! get an event with this function, one has to know its type!
	template <typename T>
	T& get(EventID event_id);

	//! In case we don't know the type of an event, we can use this function to
	//! retrieve it and then use get<Type>(...) to get the event.
	Event::Type getType(EventID event_id) const;

	//! Returns the number of all events of a certain channel.
	std::size_t numberOfEvents(channel_t ch) const;

	//! Gives a range that can be used in a range based for loop to iterate over
	//! all events in channel ch of type T.
	template <typename T>
	ContainerRange<std::vector<T>> iterateOver(channel_t ch);

	//! Returns all the group ids of sample events associated with instrument_id.
	const EventGroupIDs& getSampleEventGroupIDsOf(InstrumentID instrument_id) const;

	//! Get all the event ids of events that are in the group with event_group_id.
	const EventIDs& getEventIDsOf(EventGroupID event_group_id) const;

	//! This function should always be called before we emplace a new batch of
	//! events. This internally creates a new group id and then assigns this group
	//! id to all the events that are emplaced afterwards. The group id changes,
	//! when startAddingNewGroup is again called.
	void startAddingNewGroup(InstrumentID instrument_id = InstrumentID());

	//! Clears the whole data structure to make it ready for a new drum kit.
	void clear();

private:
	struct ChannelData
	{
		std::vector<SampleEvent> sample_events;
	};

	using ChannelEventIndex = std::size_t;

	struct EventInfo
	{
		Event::Type type;
		channel_t ch;
		ChannelEventIndex channel_event_index;

		EventInfo(Event::Type type, channel_t ch, ChannelEventIndex channel_event_index)
			: type(type), ch(ch), channel_event_index(channel_event_index) {}
	};

	struct GroupData
	{
		EventIDs event_ids;
		// Note: In the future, we could make groups just consist of events of one type.
		// Currently that is not necessary, but untested.
		// Event::Type type;

		// SampleEvent specific data
		std::size_t instrument_index;
	};

	// general data
	std::array<ChannelData, NUM_CHANNELS> channel_data_array;
	MemoryHeap<EventInfo> id_to_info;
	MemoryHeap<GroupData> id_to_group_data;

	// SampleEvent specific data
	// TODO: Make this into a vector and always have the size be the number of instruments.
	std::array<EventGroupIDs, 128> instruments_sample_event_group_ids;

	EventGroupID current_group_id;
	InstrumentID current_groups_instrument_id;

	void removeGroup(EventGroupID group_id, InstrumentID instrument_id = InstrumentID());

	template <typename T>
	T& getSample(EventInfo const& info);
};

template <typename T, typename... Args>
T& EventsDS::emplace(channel_t ch, Args&&... args)
{
	// add new event types here
	static_assert(std::is_same<T, SampleEvent>::value, "Check event type");

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
		assert(sample_event.channel == ch);
		return sample_event;
	}
}

template <typename T>
T& EventsDS::get(EventID event_id)
{
	return getSample<T>(id_to_info.get(event_id));
}

template <typename T>
ContainerRange<std::vector<T>> EventsDS::iterateOver(channel_t ch)
{
	// add new event types here
	static_assert(std::is_same<T, SampleEvent>::value, "Check event type");

	if (std::is_same<T, SampleEvent>::value)
	{
		auto& sample_events = channel_data_array[ch].sample_events;
		return ContainerRange<std::vector<T>>(sample_events.begin(), sample_events.end());
	}
}

template <typename T>
T& EventsDS::getSample(EventInfo const& info)
{
	// add new event types here
	static_assert(std::is_same<T, SampleEvent>::value, "Check event type");

	if (std::is_same<T, SampleEvent>::value)
	{
		return channel_data_array[info.ch].sample_events[info.channel_event_index];
	}
}

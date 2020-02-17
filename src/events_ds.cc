/* -*- Mode: c++ -*- */
/***************************************************************************
 *            events_ds.cc
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
#include "events_ds.h"

#include <type_traits>

void EventsDS::remove(EventID event_id)
{
	const auto& event_info = id_to_info.get(event_id);

	if (event_info.type == Event::Type::SampleEvent)
	{
		auto& sample_events = channel_data_array[event_info.ch].sample_events;
		auto& event = sample_events[event_info.channel_event_index];

		// remove from id_to_group_data and delete the group if empty
		auto& ids = id_to_group_data.get(event.group_id).event_ids;
		ids.erase(std::remove(ids.begin(), ids.end(), event_id), ids.end());
		if (id_to_group_data.get(event.group_id).event_ids.empty())
		{
			removeGroup(event.group_id, event.instrument_id);
		}

		// remove from channel_data_array
		auto& swap_element = sample_events.back();
		id_to_info.get(swap_element.id).channel_event_index = event_info.channel_event_index;
		event = swap_element;
		sample_events.pop_back();
	}

	// remove from id_to_info
	id_to_info.remove(event_id);
}

std::size_t EventsDS::numberOfEvents(channel_t ch) const
{
	auto& channel_data = channel_data_array[ch];
	return channel_data.sample_events.size();
}

const EventGroupIDs& EventsDS::getSampleEventGroupIDsOf(InstrumentID instrument_id) const
{
	return instruments_sample_event_group_ids[instrument_id];
}

const EventIDs& EventsDS::getEventIDsOf(EventGroupID event_group_id) const
{
	return id_to_group_data.get(event_group_id).event_ids;
}

void EventsDS::startAddingNewGroup(InstrumentID instrument_id)
{
	// if nothing got added for the last group, delete it
	if (current_group_id.valid() && id_to_group_data.get(current_group_id).event_ids.empty())
	{
			removeGroup(current_group_id, current_groups_instrument_id);
	}

	current_group_id = id_to_group_data.emplace();
	if (instrument_id.valid())
	{
		current_groups_instrument_id = instrument_id;
		auto& group_ids = instruments_sample_event_group_ids[instrument_id];
		group_ids.push_back(current_group_id);
		id_to_group_data.get(current_group_id).instrument_index = group_ids.size() - 1;
	}
	else
	{
		current_groups_instrument_id.invalidate();
	}
}

void EventsDS::removeGroup(EventGroupID group_id, InstrumentID instrument_id)
{
	// if we remove the current group, then invalidate it
	if (group_id == current_group_id)
	{
		current_group_id.invalidate();
		current_groups_instrument_id.invalidate();
	}

	if (instrument_id.valid())
	{
		auto instrument_index = id_to_group_data.get(group_id).instrument_index;
		auto& ids = instruments_sample_event_group_ids[instrument_id];

		id_to_group_data.get(ids.back()).instrument_index = instrument_index;
		ids[instrument_index] = ids.back();
		ids.pop_back();
	}

	id_to_group_data.remove(group_id);
}

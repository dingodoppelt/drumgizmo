/* -*- Mode: c++ -*- */
/***************************************************************************
 *            eventsdstest.cc
 *
 *  Sun Feb 16 21:38:59 CET 2020
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
#include "dgunit.h"

#include "../src/events_ds.h"

class EventsDSTest
	: public DGUnit
{
public:
	EventsDSTest()
	{
		DGUNIT_TEST(EventsDSTest::test_all);
	}

public:
	void test_all()
	{
		EventsDS events_ds;

		// group 1
		events_ds.startAddingNewGroup(42);
		events_ds.emplace<SampleEvent>(13, 13, 1.0, nullptr, "a", 42);
		events_ds.emplace<SampleEvent>(13, 13, 1.0, nullptr, "b", 42);
		events_ds.emplace<SampleEvent>(13, 13, 1.0, nullptr, "c", 42);

		DGUNIT_ASSERT(events_ds.getSampleEventGroupIDsOf(13).empty());
		DGUNIT_ASSERT(events_ds.getSampleEventGroupIDsOf(42).size() == 1);
		auto group_id = events_ds.getSampleEventGroupIDsOf(42).back();

		const auto& event_ids = events_ds.getEventIDsOf(group_id);
		DGUNIT_ASSERT(event_ids.size() == 3);

		// group 2
		events_ds.startAddingNewGroup(42);
		events_ds.emplace<SampleEvent>(13, 13, 1.0, nullptr, "d", 42);

		DGUNIT_ASSERT(events_ds.getSampleEventGroupIDsOf(42).size() == 2);

		// group 3
		events_ds.startAddingNewGroup(23);
		events_ds.emplace<SampleEvent>(7, 7, 1.0, nullptr, "foo", 23);
		events_ds.emplace<SampleEvent>(7, 7, 1.0, nullptr, "bar", 23);

		DGUNIT_ASSERT(events_ds.getSampleEventGroupIDsOf(42).size() == 2);
		DGUNIT_ASSERT(events_ds.numberOfEvents(13) == 4);
		DGUNIT_ASSERT(events_ds.numberOfEvents(7) == 2);

		// iterate over
		std::string group_concat = "";
		for (const auto& sample_event: events_ds.iterateOver<SampleEvent>(13))
		{
			group_concat.append(sample_event.group);
		}
		DGUNIT_ASSERT(group_concat == "abcd");

		// get and getType
		for (const auto& sample_event: events_ds.iterateOver<SampleEvent>(13))
		{
			DGUNIT_ASSERT(events_ds.get<SampleEvent>(sample_event.id).channel == 13);
			DGUNIT_ASSERT(events_ds.getType(sample_event.id) == Event::Type::SampleEvent);
		}

		// remove
		auto event_id = events_ds.getEventIDsOf(events_ds.getSampleEventGroupIDsOf(42).back()).back();
		events_ds.remove(event_id);
		DGUNIT_ASSERT(events_ds.getSampleEventGroupIDsOf(42).size() == 1);

		event_id = events_ds.getEventIDsOf(events_ds.getSampleEventGroupIDsOf(23).back()).back();
		events_ds.remove(event_id);
		DGUNIT_ASSERT(!events_ds.getSampleEventGroupIDsOf(23).empty());
		event_id = events_ds.getEventIDsOf(events_ds.getSampleEventGroupIDsOf(23).back()).back();
		events_ds.remove(event_id);
		DGUNIT_ASSERT(events_ds.getSampleEventGroupIDsOf(23).empty());
	}
};

// Registers the fixture into the 'registry'
static EventsDSTest test;

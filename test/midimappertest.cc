/* -*- Mode: c++ -*- */
/***************************************************************************
 *            midimappertest.cc
 *
 *  Sun Aug 8 09:55:13 CEST 2021
 *  Copyright 2021 Bent Bisballe Nyeng
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA. */
#include <uunit.h>

#include <algorithm>

#include <midimapper.h>

#include "scopedfile.h"

class MidiMapperTest
	: public uUnit
{
public:
	MidiMapperTest()
	{
		uTEST(MidiMapperTest::test);
		uTEST(MidiMapperTest::exceptional);
	}

	void test()
	{
		midimap_t midimap
		{
			{ 54, "Crash_left_tip" },
			{ 60, "Crash_left_whisker" },
			{ 55, "Crash_right_tip" },
			{ 62, "Crash_right_whisker" },
			{ 62, "Hihat_closed" },
			{ 56, "Hihat_closed" },
		};

		instrmap_t instrmap
		{
			{ "Crash_left_tip", 0 },
			{ "Crash_left_whisker", 1 },
			{ "Crash_right_tip", 2 },
			{ "Crash_right_whisker", 3 },
			{ "Hihat_closed", 4 },
		};

		ccmap_t ccmap
		{
			{ 64, 51 },
			{ 65, 52 },
			{ 66, 53 },
			{ 67, 54 },
			{ 67, 55 },
		};

		MidiMapper mapper;
		mapper.swap(instrmap, midimap, ccmap);

		{
			auto is = mapper.lookup(54);
			uASSERT_EQUAL(1u, is.size());
			uASSERT_EQUAL(0, is[0]);
		}

		{
			auto is = mapper.lookup(60);
			uASSERT_EQUAL(1u, is.size());
			uASSERT_EQUAL(1, is[0]);
		}

		{
			auto is = mapper.lookup(55);
			uASSERT_EQUAL(1u, is.size());
			uASSERT_EQUAL(2, is[0]);
		}

		{
			auto is = mapper.lookup(62);
			uASSERT_EQUAL(2u, is.size());
			// We don't care about the order, so just count the instances
			uASSERT_EQUAL(1u, std::count(is.begin(), is.end(), 3));
			uASSERT_EQUAL(1u, std::count(is.begin(), is.end(), 4));
		}

		{
			auto is = mapper.lookup(56);
			uASSERT_EQUAL(1u, is.size());
			uASSERT_EQUAL(4, is[0]);
		}

		{
			auto is = mapper.lookupCC(64);
			uASSERT_EQUAL(1u, is.size());
			uASSERT_EQUAL(51, is[0]);
		}

		{
			auto is = mapper.lookupCC(65);
			uASSERT_EQUAL(1u, is.size());
			uASSERT_EQUAL(52, is[0]);
		}

		{
			auto is = mapper.lookupCC(66);
			uASSERT_EQUAL(1u, is.size());
			uASSERT_EQUAL(53, is[0]);
		}

		{
			auto is = mapper.lookupCC(67);
			uASSERT_EQUAL(2u, is.size());
			// We don't care about the order, so just count the instances
			uASSERT_EQUAL(1u, std::count(is.begin(), is.end(), 54));
			uASSERT_EQUAL(1u, std::count(is.begin(), is.end(), 55));
		}
	}

	void exceptional()
	{
		midimap_t midimap
		{
			{ 54, "Crash_left_tip" },
			{ 60, "Crash_left_whisker_MISSING" },
			{ 55, "Crash_right_tip" },
			{ 62, "Crash_right_whisker" },
			{ 62, "Hihat_closed" },
			{ 56, "Hihat_closed" },
		};

		instrmap_t instrmap
		{
			{ "Crash_left_tip", 0 },
			{ "Crash_left_whisker", 1 },
			{ "Crash_right_tip", 2 },
			{ "Crash_right_whisker", 3 },
			{ "Hihat_closed", 4 },
		};

		ccmap_t ccmap
		{
			{ 64, 51 },
			{ 65, 52 },
			{ 66, 53 },
			{ 67, 54 },
			{ 67, 55 },
		};

		MidiMapper mapper;
		mapper.swap(instrmap, midimap, ccmap);

		// no such note id
		{
			auto is = mapper.lookup(42);
			uASSERT_EQUAL(0u, is.size());
		}

		// no such instrument
		{
			auto is = mapper.lookup(60);
			uASSERT_EQUAL(0u, is.size());
		}

		// no such cc id
		{
			auto is = mapper.lookupCC(1);
			uASSERT_EQUAL(0u, is.size());
		}

		// no such note
		{
			auto is = mapper.lookupCC(-1);
			uASSERT_EQUAL(0u, is.size());
		}
	}
};

// Registers the fixture into the 'registry'
static MidiMapperTest test;

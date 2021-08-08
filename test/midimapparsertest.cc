/* -*- Mode: c++ -*- */
/***************************************************************************
 *            midimapparsertest.cc
 *
 *  Wed Jul 25 20:37:23 CEST 2018
 *  Copyright 2018 Bent Bisballe Nyeng
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include <uunit.h>

#include <midimapparser.h>

#include "scopedfile.h"

class MidimapParserTest
	: public uUnit
{
public:
	MidimapParserTest()
	{
		uTEST(MidimapParserTest::test);
		uTEST(MidimapParserTest::invalid);
	}

	void test()
	{
		ScopedFile scoped_file(
			"<?xml version='1.0' encoding='UTF-8'?>\n" \
			"<midimap>\n" \
			"	<map note=\"54\" instr=\"Crash_left_tip\"/>\n" \
			"	<map note=\"60\" instr=\"Crash_left_whisker\"/>\n" \
			"	<map note=\"55\" instr=\"Crash_right_tip\"/>\n" \
			"	<map note=\"62\" instr=\"Crash_right_whisker\"/>\n" \
			"	<map note=\"62\" instr=\"Hihat_closed\"/>\n" \
			"	<map note=\"56\" instr=\"Hihat_closed\"/>\n" \
			"</midimap>");

		MidiMapParser parser;
		uASSERT(parser.parseFile(scoped_file.filename()));

		const auto& midimap = parser.midimap;
		uASSERT_EQUAL(6u, midimap.size());

		uASSERT_EQUAL(54, midimap[0].note_id);
		uASSERT_EQUAL(std::string("Crash_left_tip"), midimap[0].instrument_name);

		uASSERT_EQUAL(60, midimap[1].note_id);
		uASSERT_EQUAL(std::string("Crash_left_whisker"), midimap[1].instrument_name);

		uASSERT_EQUAL(55, midimap[2].note_id);
		uASSERT_EQUAL(std::string("Crash_right_tip"), midimap[2].instrument_name);

		// These next two note numbers are intentionally the same and trigger two
		// different instruments:
		uASSERT_EQUAL(62, midimap[3].note_id);
		uASSERT_EQUAL(std::string("Crash_right_whisker"), midimap[3].instrument_name);

		uASSERT_EQUAL(62, midimap[4].note_id);
		uASSERT_EQUAL(std::string("Hihat_closed"), midimap[4].instrument_name);

		uASSERT_EQUAL(56, midimap[5].note_id);
		uASSERT_EQUAL(std::string("Hihat_closed"), midimap[5].instrument_name);
	}

	void invalid()
	{
		ScopedFile scoped_file(
			"<?xml version='1.0' encoding='UTF-8'?>\n" \
			"<midimap\n" \
			"	<map note=\"54\" instr=\"Crash_left_tip\"/>\n" \
			"	<map note=\"60\" instr=\"Crash_left_whisker\"/>\n" \
			"	<map note=\"55\" instr=\"Crash_right_tip\"/>\n" \
			"	<map note=\"62\" instr=\"Crash_right_whisker\"/>\n" \
			"	<map note=\"56\" instr=\"Hihat_closed\"/>\n" \
			"</midimap>");

		MidiMapParser parser;
		uASSERT(!parser.parseFile(scoped_file.filename()));
	}
};

// Registers the fixture into the 'registry'
static MidimapParserTest test;

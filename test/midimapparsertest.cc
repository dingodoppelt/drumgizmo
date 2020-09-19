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
		uUNIT_TEST(MidimapParserTest::test);
		uUNIT_TEST(MidimapParserTest::invalid);
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
			"	<map note=\"56\" instr=\"Hihat_closed\"/>\n" \
			"</midimap>");

		MidiMapParser parser;
		uUNIT_ASSERT(parser.parseFile(scoped_file.filename()));

		uUNIT_ASSERT(parser.midimap.find(54) != parser.midimap.end());
		uUNIT_ASSERT(parser.midimap.find(60) != parser.midimap.end());
		uUNIT_ASSERT(parser.midimap.find(55) != parser.midimap.end());
		uUNIT_ASSERT(parser.midimap.find(62) != parser.midimap.end());
		uUNIT_ASSERT(parser.midimap.find(56) != parser.midimap.end());

		uUNIT_ASSERT_EQUAL(std::string("Crash_left_tip"), parser.midimap[54]);
		uUNIT_ASSERT_EQUAL(std::string("Crash_left_whisker"), parser.midimap[60]);
		uUNIT_ASSERT_EQUAL(std::string("Crash_right_tip"), parser.midimap[55]);
		uUNIT_ASSERT_EQUAL(std::string("Crash_right_whisker"), parser.midimap[62]);
		uUNIT_ASSERT_EQUAL(std::string("Hihat_closed"), parser.midimap[56]);
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
		uUNIT_ASSERT(!parser.parseFile(scoped_file.filename()));
	}
};

// Registers the fixture into the 'registry'
static MidimapParserTest test;

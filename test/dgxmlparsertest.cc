/* -*- Mode: c++ -*- */
/***************************************************************************
 *            dgxmlparsertest.cc
 *
 *  Sat Jun  9 11:37:19 CEST 2018
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
#include <cppunit/extensions/HelperMacros.h>

#include <dgxmlparser.h>
#include "scopedfile.h"

class DGXmlParserTest
	: public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(DGXmlParserTest);
	CPPUNIT_TEST(instrumentParserTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp()
	{
	}

	void tearDown()
	{
	}

	//! This just creates some drumkit.
	void instrumentParserTest()
	{
		ScopedFile scoped_file(
			"<?xml version='1.0' encoding='UTF-8'?>\n" \
			"<instrument version=\"2.0\" name=\"Snare\">\n" \
			" <channels>\n" \
			"  <channel name=\"AmbLeft\" main=\"true\">\">\n" \
			"  <channel name=\"AmbRight\" main=\"false\">\">\n" \
			"  <channel name=\"SnareBottom\">\">\n" \
			" </channels>\n" \
			" <samples>\n" \
			"  <sample name=\"Snare-1\" power=\"0.00985718\">\n" \
			"   <audiofile channel=\"AmbLeft\" file=\"1-Snare.wav\" filechannel=\"1\"/>\n" \
			"   <audiofile channel=\"AmbRight\" file=\"1-Snare.wav\" filechannel=\"2\"/>\n" \
			"   <audiofile channel=\"SnareBottom\" file=\"1-Snare.wav\" filechannel=\"12\"/>\n" \
			"   <audiofile channel=\"SnareTop\" file=\"1-Snare.wav\" filechannel=\"13\"/>\n" \
			"  </sample>\n" \
			"  <sample name=\"Snare-2\" power=\"0.0124808\">\n" \
			"   <audiofile channel=\"AmbLeft\" file=\"2-Snare.wav\" filechannel=\"1\"/>\n" \
			"   <audiofile channel=\"AmbRight\" file=\"2-Snare.wav\" filechannel=\"2\"/>\n" \
			"   <audiofile channel=\"SnareBottom\" file=\"2-Snare.wav\" filechannel=\"12\"/>\n" \
			"   <audiofile channel=\"SnareTop\" file=\"2-Snare.wav\" filechannel=\"13\"/>\n" \
			"  </sample>\n" \
			" </samples>\n" \
			"</instrument>");

		InstrumentDOM dom;
		CPPUNIT_ASSERT(probeInstrumentFile(scoped_file.filename()));
		CPPUNIT_ASSERT(parseInstrumentFile(scoped_file.filename(), dom));

		CPPUNIT_ASSERT_EQUAL(std::string("Snare"), dom.name);
		CPPUNIT_ASSERT_EQUAL(std::size_t(2), dom.samples.size());

		{
			const auto& s = dom.samples[0];
			CPPUNIT_ASSERT_EQUAL(std::string("Snare-1"), s.name);
			CPPUNIT_ASSERT_EQUAL(0.00985718, s.power);
			CPPUNIT_ASSERT_EQUAL(std::size_t(4), s.audiofiles.size());

			CPPUNIT_ASSERT_EQUAL(std::string("AmbLeft"), s.audiofiles[0].instrument_channel);
			CPPUNIT_ASSERT_EQUAL(std::string("1-Snare.wav"), s.audiofiles[0].file);
			CPPUNIT_ASSERT_EQUAL(std::size_t(1), s.audiofiles[0].filechannel);

			CPPUNIT_ASSERT_EQUAL(std::string("AmbRight"), s.audiofiles[1].instrument_channel);
			CPPUNIT_ASSERT_EQUAL(std::string("1-Snare.wav"), s.audiofiles[1].file);
			CPPUNIT_ASSERT_EQUAL(std::size_t(2), s.audiofiles[1].filechannel);

			CPPUNIT_ASSERT_EQUAL(std::string("SnareBottom"), s.audiofiles[2].instrument_channel);
			CPPUNIT_ASSERT_EQUAL(std::string("1-Snare.wav"), s.audiofiles[2].file);
			CPPUNIT_ASSERT_EQUAL(std::size_t(3), s.audiofiles[2].filechannel);

			CPPUNIT_ASSERT_EQUAL(std::string("SnareTop"), s.audiofiles[3].instrument_channel);
			CPPUNIT_ASSERT_EQUAL(std::string("1-Snare.wav"), s.audiofiles[3].file);
			CPPUNIT_ASSERT_EQUAL(std::size_t(4), s.audiofiles[3].filechannel);
		}

		{
			const auto& s = dom.samples[1];
			CPPUNIT_ASSERT_EQUAL(std::string("Snare-2"), s.name);
			CPPUNIT_ASSERT_EQUAL(0.0124808, s.power);
			CPPUNIT_ASSERT_EQUAL(std::size_t(4), s.audiofiles.size());
			CPPUNIT_ASSERT_EQUAL(std::string("AmbLeft"), s.audiofiles[0].instrument_channel);
			CPPUNIT_ASSERT_EQUAL(std::string("2-Snare.wav"), s.audiofiles[0].file);
			CPPUNIT_ASSERT_EQUAL(std::size_t(1), s.audiofiles[0].filechannel);

			CPPUNIT_ASSERT_EQUAL(std::string("AmbRight"), s.audiofiles[1].instrument_channel);
			CPPUNIT_ASSERT_EQUAL(std::string("2-Snare.wav"), s.audiofiles[1].file);
			CPPUNIT_ASSERT_EQUAL(std::size_t(2), s.audiofiles[1].filechannel);

			CPPUNIT_ASSERT_EQUAL(std::string("SnareBottom"), s.audiofiles[2].instrument_channel);
			CPPUNIT_ASSERT_EQUAL(std::string("2-Snare.wav"), s.audiofiles[2].file);
			CPPUNIT_ASSERT_EQUAL(std::size_t(3), s.audiofiles[2].filechannel);

			CPPUNIT_ASSERT_EQUAL(std::string("SnareTop"), s.audiofiles[3].instrument_channel);
			CPPUNIT_ASSERT_EQUAL(std::string("2-Snare.wav"), s.audiofiles[3].file);
			CPPUNIT_ASSERT_EQUAL(std::size_t(4), s.audiofiles[3].filechannel);
		}

		CPPUNIT_ASSERT_EQUAL(std::size_t(3), dom.instrument_channels.size());
		CPPUNIT_ASSERT_EQUAL(std::string("AmbLeft"), dom.instrument_channels[0].name);
		CPPUNIT_ASSERT_EQUAL(main_state_t::is_main, dom.instrument_channels[0].main);
		CPPUNIT_ASSERT_EQUAL(std::string("AmbRight"), dom.instrument_channels[1].name);
		CPPUNIT_ASSERT_EQUAL(main_state_t::is_not_main, dom.instrument_channels[1].main);
		CPPUNIT_ASSERT_EQUAL(std::string("SnareBottom"), dom.instrument_channels[2].name);
		CPPUNIT_ASSERT_EQUAL(main_state_t::unset, dom.instrument_channels[2].main);
	}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(DGXmlParserTest);

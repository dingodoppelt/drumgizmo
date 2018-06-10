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
	CPPUNIT_TEST(drumkitParserTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp()
	{
	}

	void tearDown()
	{
	}

	void instrumentParserTest()
	{
		ScopedFile scoped_file(
			"<?xml version='1.0' encoding='UTF-8'?>\n" \
			"<instrument version=\"2.0\" name=\"Snare\">\n" \
			" <channels>\n" \
			"  <channel name=\"AmbLeft\" main=\"true\"/>\n" \
			"  <channel name=\"AmbRight\" main=\"false\"/>\n" \
			"  <channel name=\"SnareBottom\"/>\n" \
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
			CPPUNIT_ASSERT_EQUAL(std::size_t(12), s.audiofiles[2].filechannel);

			CPPUNIT_ASSERT_EQUAL(std::string("SnareTop"), s.audiofiles[3].instrument_channel);
			CPPUNIT_ASSERT_EQUAL(std::string("1-Snare.wav"), s.audiofiles[3].file);
			CPPUNIT_ASSERT_EQUAL(std::size_t(13), s.audiofiles[3].filechannel);
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
			CPPUNIT_ASSERT_EQUAL(std::size_t(12), s.audiofiles[2].filechannel);

			CPPUNIT_ASSERT_EQUAL(std::string("SnareTop"), s.audiofiles[3].instrument_channel);
			CPPUNIT_ASSERT_EQUAL(std::string("2-Snare.wav"), s.audiofiles[3].file);
			CPPUNIT_ASSERT_EQUAL(std::size_t(13), s.audiofiles[3].filechannel);
		}

		CPPUNIT_ASSERT_EQUAL(std::size_t(3), dom.instrument_channels.size());
		CPPUNIT_ASSERT_EQUAL(std::string("AmbLeft"), dom.instrument_channels[0].name);
		CPPUNIT_ASSERT(main_state_t::is_main == dom.instrument_channels[0].main);
		CPPUNIT_ASSERT_EQUAL(std::string("AmbRight"), dom.instrument_channels[1].name);
		CPPUNIT_ASSERT(main_state_t::is_not_main == dom.instrument_channels[1].main);
		CPPUNIT_ASSERT_EQUAL(std::string("SnareBottom"), dom.instrument_channels[2].name);
		CPPUNIT_ASSERT(main_state_t::unset == dom.instrument_channels[2].main);
	}

	void drumkitParserTest()
	{
		ScopedFile scoped_file(
			"<?xml version='1.0' encoding='UTF-8'?>\n" \
			"<drumkit name=\"CrocellKit\" description=\"my description\" samplerate=\"48000\" version=\"2.0.0\">\n" \
			"  <channels>\n" \
			"   <channel name=\"AmbLeft\"/>\n" \
			"   <channel name=\"AmbRight\"/>\n" \
			"   <channel name=\"SnareTop\"/>\n" \
			"   <channel name=\"SnareBottom\"/>\n" \
			"  </channels>\n" \
			"  <instruments>\n" \
			"    <instrument name=\"Snare1\" group=\"somegroup\" file=\"foo.wav\">\n" \
			"      <channelmap in=\"AmbLeft-in\" out=\"AmbLeft\" main=\"true\"/>\n" \
			"      <channelmap in=\"AmbRight-in\" out=\"AmbRight\" main=\"true\"/>\n" \
			"      <channelmap in=\"SnareTop-in\" out=\"SnareTop\"/>\n" \
			"      <channelmap in=\"SnareBottom-in\" out=\"SnareBottom\"/>\n" \
			"    </instrument>\n" \
			"    <instrument name=\"Snare2\" file=\"bar.wav\">\n" \
			"      <channelmap in=\"AmbLeft2-in\" out=\"AmbLeft\" main=\"false\"/>\n" \
			"      <channelmap in=\"AmbRight2-in\" out=\"AmbRight\" main=\"false\"/>\n" \
			"      <channelmap in=\"SnareTop2-in\" out=\"SnareTop\"/>\n" \
			"      <channelmap in=\"SnareBottom2-in\" out=\"SnareBottom\"/>\n" \
			"    </instrument>\n" \
			"  </instruments>\n" \
			"</drumkit>");

		DrumkitDOM dom;
		CPPUNIT_ASSERT(probeDrumkitFile(scoped_file.filename()));
		CPPUNIT_ASSERT(parseDrumkitFile(scoped_file.filename(), dom));

		CPPUNIT_ASSERT_EQUAL(std::string("CrocellKit"), dom.name);
		CPPUNIT_ASSERT_EQUAL(std::string("my description"), dom.description);
		CPPUNIT_ASSERT_EQUAL(48000.0, dom.samplerate);
		CPPUNIT_ASSERT_EQUAL(std::size_t(2), dom.instruments.size());
		{
			const auto& instr = dom.instruments[0];
			CPPUNIT_ASSERT_EQUAL(std::string("Snare1"), instr.name);
			CPPUNIT_ASSERT_EQUAL(std::string("foo.wav"), instr.file);
			CPPUNIT_ASSERT_EQUAL(std::string("somegroup"), instr.group);
			CPPUNIT_ASSERT_EQUAL(std::size_t(4), instr.channel_map.size());

			CPPUNIT_ASSERT_EQUAL(std::string("AmbLeft-in"), instr.channel_map[0].in);
			CPPUNIT_ASSERT_EQUAL(std::string("AmbRight-in"), instr.channel_map[1].in);
			CPPUNIT_ASSERT_EQUAL(std::string("SnareTop-in"), instr.channel_map[2].in);
			CPPUNIT_ASSERT_EQUAL(std::string("SnareBottom-in"), instr.channel_map[3].in);

			CPPUNIT_ASSERT_EQUAL(std::string("AmbLeft"), instr.channel_map[0].out);
			CPPUNIT_ASSERT_EQUAL(std::string("AmbRight"), instr.channel_map[1].out);
			CPPUNIT_ASSERT_EQUAL(std::string("SnareTop"), instr.channel_map[2].out);
			CPPUNIT_ASSERT_EQUAL(std::string("SnareBottom"), instr.channel_map[3].out);

			CPPUNIT_ASSERT(main_state_t::is_main == instr.channel_map[0].main);
			CPPUNIT_ASSERT(main_state_t::is_main == instr.channel_map[1].main);
			CPPUNIT_ASSERT(main_state_t::unset == instr.channel_map[2].main);
			CPPUNIT_ASSERT(main_state_t::unset == instr.channel_map[3].main);
		}
		{
			const auto& instr = dom.instruments[1];
			CPPUNIT_ASSERT_EQUAL(std::string("Snare2"), instr.name);
			CPPUNIT_ASSERT_EQUAL(std::string("bar.wav"), instr.file);
			CPPUNIT_ASSERT_EQUAL(std::string(""), instr.group);
			CPPUNIT_ASSERT_EQUAL(std::size_t(4), instr.channel_map.size());

			CPPUNIT_ASSERT_EQUAL(std::string("AmbLeft2-in"), instr.channel_map[0].in);
			CPPUNIT_ASSERT_EQUAL(std::string("AmbRight2-in"), instr.channel_map[1].in);
			CPPUNIT_ASSERT_EQUAL(std::string("SnareTop2-in"), instr.channel_map[2].in);
			CPPUNIT_ASSERT_EQUAL(std::string("SnareBottom2-in"), instr.channel_map[3].in);

			CPPUNIT_ASSERT_EQUAL(std::string("AmbLeft"), instr.channel_map[0].out);
			CPPUNIT_ASSERT_EQUAL(std::string("AmbRight"), instr.channel_map[1].out);
			CPPUNIT_ASSERT_EQUAL(std::string("SnareTop"), instr.channel_map[2].out);
			CPPUNIT_ASSERT_EQUAL(std::string("SnareBottom"), instr.channel_map[3].out);

			CPPUNIT_ASSERT(main_state_t::is_not_main == instr.channel_map[0].main);
			CPPUNIT_ASSERT(main_state_t::is_not_main == instr.channel_map[1].main);
			CPPUNIT_ASSERT(main_state_t::unset == instr.channel_map[2].main);
			CPPUNIT_ASSERT(main_state_t::unset == instr.channel_map[3].main);
		}

		CPPUNIT_ASSERT_EQUAL(std::size_t(4), dom.channels.size());
		CPPUNIT_ASSERT_EQUAL(std::string("AmbLeft"), dom.channels[0].name);
		CPPUNIT_ASSERT_EQUAL(std::string("AmbRight"), dom.channels[1].name);
		CPPUNIT_ASSERT_EQUAL(std::string("SnareTop"), dom.channels[2].name);
		CPPUNIT_ASSERT_EQUAL(std::string("SnareBottom"), dom.channels[3].name);
	}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(DGXmlParserTest);

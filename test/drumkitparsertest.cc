/* -*- Mode: c++ -*- */
/***************************************************************************
 *            drumkitparsertest.cc
 *
 *  Wed Jun  6 20:39:37 CEST 2018
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

#include <drumkitparser.h>
#include <drumkit.h>

#include "scopedfile.h"

class DrumkitParserTest
	: public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(DrumkitParserTest);
	CPPUNIT_TEST(testTest);
	CPPUNIT_TEST_SUITE_END();

	Settings settings;
	Random rand;

public:
	void setUp()
	{
	}

	void tearDown()
	{
	}

	//! This just creates some drumkit.
	void testTest()
	{
		ScopedFile scoped_instrument_file1(
			"<?xml version='1.0' encoding='UTF-8'?>\n" \
			"<instrument version=\"2.0\" name=\"Snare\">\n" \
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

		ScopedFile scoped_instrument_file2(
			"<?xml version='1.0' encoding='UTF-8'?>\n" \
			"<instrument version=\"2.0\" name=\"Snare\">\n" \
			" <samples>\n" \
			"  <sample name=\"Snare-1\" power=\"0.00985718\">\n" \
			"   <audiofile channel=\"AmbLeft2\" file=\"1-Snare.wav\" filechannel=\"1\"/>\n" \
			"   <audiofile channel=\"AmbRight2\" file=\"1-Snare.wav\" filechannel=\"2\"/>\n" \
			"   <audiofile channel=\"SnareBottom2\" file=\"1-Snare.wav\" filechannel=\"12\"/>\n" \
			"   <audiofile channel=\"SnareTop2\" file=\"1-Snare.wav\" filechannel=\"13\"/>\n" \
			"  </sample>\n" \
			"  <sample name=\"Snare-2\" power=\"0.0124808\">\n" \
			"   <audiofile channel=\"AmbLeft2\" file=\"2-Snare.wav\" filechannel=\"1\"/>\n" \
			"   <audiofile channel=\"AmbRight2\" file=\"2-Snare.wav\" filechannel=\"2\"/>\n" \
			"   <audiofile channel=\"SnareBottom2\" file=\"2-Snare.wav\" filechannel=\"12\"/>\n" \
			"   <audiofile channel=\"SnareTop2\" file=\"2-Snare.wav\" filechannel=\"13\"/>\n" \
			"  </sample>\n" \
			" </samples>\n" \
			"</instrument>");

		ScopedFile scoped_file(
			std::string(
			"<?xml version='1.0' encoding='UTF-8'?>\n" \
			"<drumkit name=\"CrocellKit\" description=\"my description\" samplerate=\"48000\" version=\"2.0.0\">\n" \
			"  <channels>\n" \
			"   <channel name=\"AmbLeft\"/>\n" \
			"   <channel name=\"AmbRight\"/>\n" \
			"   <channel name=\"SnareTop\"/>\n" \
			"   <channel name=\"SnareBottom\"/>\n" \
			"  </channels>\n" \
			"  <instruments>\n" \
			"    <instrument name=\"Snare1\" file=\"") + scoped_instrument_file1.filename() + std::string("\">\n" \
			"      <channelmap in=\"AmbLeft\" out=\"AmbLeft\" main=\"true\"/>\n" \
			"      <channelmap in=\"AmbRight\" out=\"AmbRight\" main=\"true\"/>\n" \
			"      <channelmap in=\"SnareTop\" out=\"SnareTop\"/>\n" \
			"      <channelmap in=\"SnareBottom\" out=\"SnareBottom\"/>\n" \
			"    </instrument>\n" \
			"    <instrument name=\"Snare2\" file=\"") + scoped_instrument_file2.filename() + std::string("\">\n" \
			"      <channelmap in=\"AmbLeft2\" out=\"AmbLeft\" main=\"true\"/>\n" \
			"      <channelmap in=\"AmbRight2\" out=\"AmbRight\" main=\"true\"/>\n" \
			"      <channelmap in=\"SnareTop2\" out=\"SnareTop\"/>\n" \
			"      <channelmap in=\"SnareBottom2\" out=\"SnareBottom\"/>\n" \
			"    </instrument>\n" \
			"  </instruments>\n" \
			"</drumkit>"));

		DrumKit drumkit;
		DrumKitParser parser(settings, drumkit, rand);

		CPPUNIT_ASSERT_EQUAL(0, parser.parseFile(scoped_file.filename()));

		CPPUNIT_ASSERT_EQUAL(std::size_t(2), drumkit.instruments.size());
		CPPUNIT_ASSERT_EQUAL(std::size_t(4), drumkit.channels.size());

		CPPUNIT_ASSERT_EQUAL(std::string("AmbLeft"), drumkit.channels[0].name);
		CPPUNIT_ASSERT_EQUAL(std::string("AmbRight"), drumkit.channels[1].name);
		CPPUNIT_ASSERT_EQUAL(std::string("SnareTop"), drumkit.channels[2].name);
		CPPUNIT_ASSERT_EQUAL(std::string("SnareBottom"), drumkit.channels[3].name);

		CPPUNIT_ASSERT_EQUAL(std::string("CrocellKit"), drumkit._name);
		CPPUNIT_ASSERT_EQUAL(std::string("my description"), drumkit._description);
		CPPUNIT_ASSERT_EQUAL(std::size_t(48000), drumkit._samplerate);

		CPPUNIT_ASSERT(VersionStr("2.0.0") == drumkit._version);
	}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(DrumkitParserTest);

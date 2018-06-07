/* -*- Mode: c++ -*- */
/***************************************************************************
 *            instrumentparsertest.cc
 *
 *  Wed Jun  6 12:11:16 CEST 2018
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

#include <instrumentparser.h>
#include "scopedfile.h"

class InstrumentParserTest
	: public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(InstrumentParserTest);
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
		ScopedFile scoped_file(
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
		Instrument instrument(settings, rand);
		InstrumentParser parser(instrument, settings);
		CPPUNIT_ASSERT_EQUAL(0, parser.parseFile(scoped_file.filename()));

		CPPUNIT_ASSERT_EQUAL(std::string(""), instrument._group);
		CPPUNIT_ASSERT_EQUAL(std::string("Snare"), instrument._name);
		CPPUNIT_ASSERT_EQUAL(std::string(""), instrument._description);

		CPPUNIT_ASSERT(VersionStr("2.0.0") == instrument.version);

		// NOTE: instrument.samples are the sample map belonging to version 1.0
		CPPUNIT_ASSERT_EQUAL(std::size_t(2), instrument.samplelist.size());
		{
			const auto& sample = *instrument.samplelist[0];
			CPPUNIT_ASSERT_EQUAL(std::string("Snare-1"), sample.name);
			CPPUNIT_ASSERT_EQUAL(0.00985718f, sample.power);
			CPPUNIT_ASSERT_EQUAL(std::size_t(4), sample.audiofiles.size());
			for(const auto& audiofile : sample.audiofiles)
			{
				CPPUNIT_ASSERT_EQUAL(std::string("/tmp/1-Snare.wav"), audiofile.second->filename);
				switch(audiofile.second->filechannel)
				{
					// NOTE: Channel numbers are zero based - they are 1 based in the xml
				case 0:
					CPPUNIT_ASSERT_EQUAL(std::string("AmbLeft"),
					                     audiofile.second->instrument_channel->name);
					break;
				case 1:
					CPPUNIT_ASSERT_EQUAL(std::string("AmbRight"),
					                     audiofile.second->instrument_channel->name);
					break;
				case 11:
					CPPUNIT_ASSERT_EQUAL(std::string("SnareBottom"),
					                     audiofile.second->instrument_channel->name);
					break;
				case 12:
					CPPUNIT_ASSERT_EQUAL(std::string("SnareTop"),
					                     audiofile.second->instrument_channel->name);
					break;
				default:
					CPPUNIT_ASSERT(false);
					break;
				}
			}
		}

		{
			const auto& sample = *instrument.samplelist[1];
			CPPUNIT_ASSERT_EQUAL(std::string("Snare-2"), sample.name);
			CPPUNIT_ASSERT_EQUAL(0.0124808f, sample.power);
			CPPUNIT_ASSERT_EQUAL(std::size_t(4), sample.audiofiles.size());
			for(const auto& audiofile : sample.audiofiles)
			{
				CPPUNIT_ASSERT_EQUAL(std::string("/tmp/2-Snare.wav"), audiofile.second->filename);
				switch(audiofile.second->filechannel)
				{
					// NOTE: Channel numbers are zero based - they are 1 based in the xml
				case 0:
					CPPUNIT_ASSERT_EQUAL(std::string("AmbLeft"),
					                     audiofile.second->instrument_channel->name);
					break;
				case 1:
					CPPUNIT_ASSERT_EQUAL(std::string("AmbRight"),
					                     audiofile.second->instrument_channel->name);
					break;
				case 11:
					CPPUNIT_ASSERT_EQUAL(std::string("SnareBottom"),
					                     audiofile.second->instrument_channel->name);
					break;
				case 12:
					CPPUNIT_ASSERT_EQUAL(std::string("SnareTop"),
					                     audiofile.second->instrument_channel->name);
					break;
				default:
					CPPUNIT_ASSERT(false);
					break;
				}
			}
		}

//CPPUNIT_ASSERT(ref.samples.values == instrument.samples.values);
		//std::vector<Sample*> samplelist;
		//std::deque<InstrumentChannel> instrument_channels;
		//
		//size_t lastpos;
		//float mod;
		//Settings& settings;
		//Random& rand;
		//PowerList powerlist;
	}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(InstrumentParserTest);

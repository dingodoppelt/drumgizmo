/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            lv2.cc
 *
 *  Thu Feb 12 14:55:41 CET 2015
 *  Copyright 2015 Bent Bisballe Nyeng
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

#include <unistd.h>
#include <memory.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "drumkit_creator.h"
#include "lv2_test_host.h"

#define DG_URI "http://drumgizmo.org/lv2"

enum class Ports {
	FreeWheel = 0,
	Latency,
	MidiPort,
	AudioPortOffset,
};

/**
 * Tests that should be performed:
 * -------------------------------
 * - Run without port connects (shouldn't crash)
 * - Run without output ports connects (shouldn't crash)
 * - Run with buffer size 0
 * - Run with VERY LARGE buffer size (>1MB?)
 * - Run with buffer size a prime number (and thereby not power of 2)
 * - Run with HUGE number of midi events in one buffer (10000)
 */
class test_lv2 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(test_lv2);
	CPPUNIT_TEST(open_and_verify);
	CPPUNIT_TEST(run_no_ports_connected);
	CPPUNIT_TEST(run_no_output_ports_connected);
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST_SUITE_END();

	DrumkitCreator drumkit_creator;

public:
	void setUp() {}
	void tearDown() {}

	void open_and_verify()
	{
		int res;

		LV2TestHost h(LV2_PATH);

		res = h.open(DG_URI);
		CPPUNIT_ASSERT_EQUAL(0, res);

		res = h.verify();
		CPPUNIT_ASSERT_EQUAL(0, res);

		res = h.close();
		CPPUNIT_ASSERT_EQUAL(0, res);
	}

	void run_no_ports_connected()
	{
		int res;

		LV2TestHost h(LV2_PATH);

		res = h.open(DG_URI);
		CPPUNIT_ASSERT_EQUAL(0, res);

		res = h.verify();
		CPPUNIT_ASSERT_EQUAL(0, res);

		res = h.createInstance(44100);
		CPPUNIT_ASSERT_EQUAL(0, res);

		const char config_fmt[] =
			"<config>\n"
			"  <value name=\"drumkitfile\">%s</value>\n"
			"  <value name=\"midimapfile\">%s</value>\n"
			"  <value name=\"enable_velocity_modifier\">%s</value>\n"
			"  <value name=\"velocity_modifier_falloff\">%f</value>\n"
			"  <value name=\"velocity_modifier_weight\">%f</value>\n"
			"  <value name=\"enable_velocity_randomiser\">%s</value>\n"
			"  <value name=\"velocity_randomiser_weight\">%f</value>\n"
			"  <value name=\"enable_resampling\">%s</value>\n"
			"</config>";

		// Create drumkit
		auto kit1_file = drumkit_creator.createStdKit("kit1");

		auto midimapfile = drumkit_creator.createStdMidimap("midimap");
		bool enable_velocity_modifier = true;
		float velocity_modifier_falloff = 0.5;
		float velocity_modifier_weight = 0.25;
		bool enable_velocity_randomiser = false;
		float velocity_randomiser_weight = 0.1;
		bool enable_resampling = false;

		char config[sizeof(config_fmt) * 2];
		sprintf(config, config_fmt,
		        kit1_file.c_str(),
		        midimapfile.c_str(),
		        enable_velocity_modifier?"true":"false",
		        velocity_modifier_falloff,
		        velocity_modifier_weight,
		        enable_velocity_randomiser?"true":"false",
		        velocity_randomiser_weight,
		        enable_resampling?"true":"false");

		res = h.loadConfig(config, strlen(config));
		CPPUNIT_ASSERT_EQUAL(0, res);

		// run for 1 samples to trigger kit loading
		res = h.run(1);
		CPPUNIT_ASSERT_EQUAL(0, res);
		usleep(1000); // wait for kit to get loaded (async),

		res = h.run(100);
		CPPUNIT_ASSERT_EQUAL(0, res);

		res = h.destroyInstance();
		CPPUNIT_ASSERT_EQUAL(0, res);

		res = h.close();
		CPPUNIT_ASSERT_EQUAL(0, res);
	}

	void run_no_output_ports_connected()
	{
		int res;

		LV2TestHost h(LV2_PATH);

		res = h.open(DG_URI);
		CPPUNIT_ASSERT_EQUAL(0, res);

		res = h.verify();
		CPPUNIT_ASSERT_EQUAL(0, res);

		res = h.createInstance(44100);
		CPPUNIT_ASSERT_EQUAL(0, res);

		const char config_fmt[] =
			"<config>\n"
			"  <value name=\"drumkitfile\">%s</value>\n"
			"  <value name=\"midimapfile\">%s</value>\n"
			"  <value name=\"enable_velocity_modifier\">%s</value>\n"
			"  <value name=\"velocity_modifier_falloff\">%f</value>\n"
			"  <value name=\"velocity_modifier_weight\">%f</value>\n"
			"  <value name=\"enable_velocity_randomiser\">%s</value>\n"
			"  <value name=\"velocity_randomiser_weight\">%f</value>\n"
			"  <value name=\"enable_resampling\">%s</value>\n"
			"</config>";

		// Create drumkit
		auto kit1_file = drumkit_creator.createStdKit("kit1");

		auto midimapfile = drumkit_creator.createStdMidimap("midimap");
		bool enable_velocity_modifier = true;
		float velocity_modifier_falloff = 0.5;
		float velocity_modifier_weight = 0.25;
		bool enable_velocity_randomiser = false;
		float velocity_randomiser_weight = 0.1;
		bool enable_resampling = false;

		char config[sizeof(config_fmt) * 2];
		sprintf(config, config_fmt,
		        kit1_file.c_str(),
		        midimapfile.c_str(),
		        enable_velocity_modifier?"true":"false",
		        velocity_modifier_falloff,
		        velocity_modifier_weight,
		        enable_velocity_randomiser?"true":"false",
		        velocity_randomiser_weight,
		        enable_resampling?"true":"false");

		res = h.loadConfig(config, strlen(config));
		CPPUNIT_ASSERT_EQUAL(0, res);

		// Port buffers:
		char sequence_buffer[4096];
		bool freeWheel = false;

		// Free wheel port
		res = h.connectPort((int)Ports::FreeWheel, (void*)&freeWheel);

		LV2TestHost::Sequence seq(sequence_buffer, sizeof(sequence_buffer));
		res = h.connectPort((int)Ports::MidiPort, seq.data());
		CPPUNIT_ASSERT_EQUAL(0, res);

		// run for 1 samples to trigger kit loading
		res = h.run(1);
		CPPUNIT_ASSERT_EQUAL(0, res);
		usleep(1000); // wait for kit to get loaded (async),

		seq.addMidiNote(5, 1, 127);
		res = h.run(100);
		CPPUNIT_ASSERT_EQUAL(0, res);

		res = h.destroyInstance();
		CPPUNIT_ASSERT_EQUAL(0, res);

		res = h.close();
		CPPUNIT_ASSERT_EQUAL(0, res);
	}

	void test1()
	{
		int res;

		LV2TestHost h(LV2_PATH);

		res = h.open(DG_URI);
		CPPUNIT_ASSERT_EQUAL(0, res);

		res = h.verify();
		CPPUNIT_ASSERT_EQUAL(0, res);

		res = h.createInstance(44100);
		CPPUNIT_ASSERT_EQUAL(0, res);

		const char config_fmt[] =
			"<config>\n"
			"  <value name=\"drumkitfile\">%s</value>\n"
			"  <value name=\"midimapfile\">%s</value>\n"
			"  <value name=\"enable_velocity_modifier\">%s</value>\n"
			"  <value name=\"velocity_modifier_falloff\">%f</value>\n"
			"  <value name=\"velocity_modifier_weight\">%f</value>\n"
			"  <value name=\"enable_velocity_randomiser\">%s</value>\n"
			"  <value name=\"velocity_randomiser_weight\">%f</value>\n"
			"  <value name=\"enable_resampling\">%s</value>\n"
			"</config>";

		// Create drumkit
		auto kit1_file = drumkit_creator.createStdKit("kit1");

		auto midimapfile = drumkit_creator.createStdMidimap("midimap");
		bool enable_velocity_modifier = true;
		float velocity_modifier_falloff = 0.5;
		float velocity_modifier_weight = 0.25;
		bool enable_velocity_randomiser = false;
		float velocity_randomiser_weight = 0.1;
		bool enable_resampling = false;

		char config[sizeof(config_fmt) * 2];
		sprintf(config, config_fmt,
		        kit1_file.c_str(),
		        midimapfile.c_str(),
		        enable_velocity_modifier?"true":"false",
		        velocity_modifier_falloff,
		        velocity_modifier_weight,
		        enable_velocity_randomiser?"true":"false",
		        velocity_randomiser_weight,
		        enable_resampling?"true":"false");

		res = h.loadConfig(config, strlen(config));
		CPPUNIT_ASSERT_EQUAL(0, res);

		// Port buffers:
		char sequence_buffer[4096];
		float pcm_buffer[16][10];
		bool freeWheel = true;

		// Free wheel port
		res = h.connectPort((int)Ports::FreeWheel, (void*)&freeWheel);

		LV2TestHost::Sequence seq(sequence_buffer, sizeof(sequence_buffer));
		res = h.connectPort((int)Ports::MidiPort, seq.data());
		CPPUNIT_ASSERT_EQUAL(0, res);

		for(int i = 0; i < 16; ++i)
		{
			for(int j = 0; j < 10; ++j)
			{
				pcm_buffer[i][j] = 0.42;
			}
			res += h.connectPort((int)Ports::AudioPortOffset + i, pcm_buffer[i]);
		}
		CPPUNIT_ASSERT_EQUAL(0, res);

		// run for 1 samples to trigger kit loading
		res = h.run(1);
		CPPUNIT_ASSERT_EQUAL(0, res);
		sleep(1); // wait for kit to get loaded (async),

		seq.addMidiNote(5, 1, 127);
		for(int i = 0; i < 10; i++)
		{
			res = h.run(10);
			usleep(1000);
			CPPUNIT_ASSERT_EQUAL(0, res);

			//printf("Iteration:\n");
			//for(int k = 0; k < 16; k++) {
			//	printf("#%d ", k);
			//	for(int j = 0; j < 10; j++) printf("[%f]", pcm_buffer[k][j]);
			//	printf("\n");
			//}
			//printf("\n");

			seq.clear();
		}


		seq.addMidiNote(5, 1, 127);
		res = h.run(10);
		usleep(1000);
		CPPUNIT_ASSERT_EQUAL(0, res);

		/*
		printf("Iteration:\n");
		for(int k = 0; k < 4; k++) {
			printf("#%d ", k);
			for(int j = 0; j < 10; j++) printf("[%f]", pcm_buffer[k][j]);
			printf("\n");
		}
		printf("\n");
		*/

		union {
			float f;
			unsigned int u;
		} comp_val;

		comp_val.u = 1040744448; // floating point value 0.133301....

		for(int k = 0; k < 4; k++)
		{
			for(int j = 0; j < 10; j++)
			{
				CPPUNIT_ASSERT_EQUAL(((j==5)?comp_val.f:0), pcm_buffer[k][j]);
			}
		}
		seq.clear();

		res = h.destroyInstance();
		CPPUNIT_ASSERT_EQUAL(0, res);

		res = h.close();
		CPPUNIT_ASSERT_EQUAL(0, res);
	}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(test_lv2);

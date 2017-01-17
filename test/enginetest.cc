/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            engine.cc
 *
 *  Fri Nov 29 18:09:02 CET 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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

#include <drumgizmo.h>
#include <unistd.h>

#include "drumkit_creator.h"

class AudioOutputEngineDummy
	: public AudioOutputEngine
{
public:
	bool init(const Channels& channels) { return true; }

	void setParm(const std::string& parm, const std::string& value) {}

	bool start() { return true; }
	void stop() {}

	void pre(size_t nsamples) {}
	void run(int ch, sample_t *samples, size_t nsamples) {}
	void post(size_t nsamples) {}
	bool isFreewheeling() const { return true; }
};

class AudioInputEngineDummy
	: public AudioInputEngine
{
public:
	bool init(const Instruments& instruments) { return true; }

	void setParm(const std::string& parm, const std::string& value) {}

	bool start() { return true; }
	void stop() {}

	void pre() {}
	void run(size_t pos, size_t len, std::vector<event_t>& events) {}
	void post() {}
	bool isFreewheeling() const { return true; }
};

class test_engine : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(test_engine);
	CPPUNIT_TEST(loading);
	CPPUNIT_TEST_SUITE_END();

	DrumkitCreator drumkit_creator;

public:
	void setUp() {}
	void tearDown() {}

	void loading()
	{
		Settings settings;
		AudioOutputEngineDummy oe;
		AudioInputEngineDummy ie;
		DrumGizmo dg(settings, oe, ie);
		dg.setFrameSize(100);

		// Create drumkits
		auto kit1_file = drumkit_creator.createStdKit("kit1");
		auto kit2_file = drumkit_creator.createStdKit("kit2");

		// Switch kits emmidiately without giving the loader time to work:
		for(int i = 0; i < 100; ++i)
		{
			settings.drumkit_file.store(kit1_file);
			settings.drumkit_file.store(kit2_file);
		}

		// Switch kits with small delay giving the loader time to work a little:
		for(int i = 0; i < 100; ++i)
		{
			settings.drumkit_file.store(kit1_file);
			usleep(100);
			settings.drumkit_file.store(kit2_file);
			usleep(100);
		}

		// Switch kits with bigger delay giving the loader time to finish
		for(int i = 0; i < 100; ++i)
		{
			settings.drumkit_file.store(kit1_file);
			usleep(10000);
			settings.drumkit_file.store(kit2_file);
			usleep(10000);
		}
	}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(test_engine);

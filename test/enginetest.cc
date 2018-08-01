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
#include "dgunit.h"

#include <thread>
#include <chrono>

#include <drumgizmo.h>

#include "drumkit_creator.h"

class AudioOutputEngineDummy
	: public AudioOutputEngine
{
public:
	bool init(const Channels& channels) override { return true; }

	void setParm(const std::string& parm, const std::string& value) override {}

	bool start() override { return true; }
	void stop() override {}

	void pre(size_t nsamples) override {}
	void run(int ch, sample_t *samples, size_t nsamples) override {}
	void post(size_t nsamples) override {}
	std::size_t getSamplerate() const override { return 44100; }
	bool isFreewheeling() const override { return true; }
};

class AudioInputEngineDummy
	: public AudioInputEngine
{
public:
	bool init(const Instruments& instruments) override { return true; }

	void setParm(const std::string& parm, const std::string& value) override {}

	bool start() override { return true; }
	void stop() override {}

	void pre() override {}
	void run(size_t pos, size_t len, std::vector<event_t>& events) override {}
	void post() override {}
	void setSampleRate(double sample_rate) override {}
	bool isFreewheeling() const override { return true; }
};

class test_engine : public DGUnit
{
public:
	test_engine()
	{
		DGUNIT_TEST(test_engine::loading);
	}

	DrumkitCreator drumkit_creator;

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
			std::this_thread::sleep_for(std::chrono::microseconds(100));
			settings.drumkit_file.store(kit2_file);
			std::this_thread::sleep_for(std::chrono::microseconds(100));
		}

		// Switch kits with bigger delay giving the loader time to finish
		for(int i = 0; i < 100; ++i)
		{
			settings.drumkit_file.store(kit1_file);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			settings.drumkit_file.store(kit2_file);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
};

// Registers the fixture into the 'registry'
static test_engine test;

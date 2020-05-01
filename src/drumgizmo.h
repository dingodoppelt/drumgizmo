/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumgizmo.h
 *
 *  Thu Sep 16 10:24:40 CEST 2010
 *  Copyright 2010 Bent Bisballe Nyeng
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
#pragma once

#include <config.h>

#include <string>
#include <list>
#include <array>
#include <memory>

#include "audiooutputengine.h"
#include "audioinputengine.h"
#include "events.h"
#include "events_ds.h"
#include "audiofile.h"
#include "drumgizmoconf.h"
#include "drumkit.h"
#include "drumkitloader.h"
#include "audiocache.h"
#include "settings.h"
#include "inputprocessor.h"
#include "zrwrapper.h"

#define REFSFILE "refs.conf"

class DrumGizmo
{
public:
	DrumGizmo(Settings& settings,
	          AudioOutputEngine& outputengine,
	          AudioInputEngine& inputengine);
	virtual ~DrumGizmo();

	bool init();

	bool run(size_t pos, sample_t *samples, size_t nsamples);
	void stop();

	void renderSampleEvent(SampleEvent& evt, int pos, sample_t *s, std::size_t sz);
	void getSamples(int ch, int pos, sample_t *s, size_t sz);

	//! Get the current engine latency in samples.
	std::size_t getLatency() const;

	float samplerate();
	void setSamplerate(float samplerate, float resample_quality = 0.7f);

	void setFrameSize(size_t framesize);

	void setFreeWheel(bool freewheel);

	void setRandomSeed(unsigned int seed);

private:
	static constexpr int MAX_RESAMPLER_BUFFER_SIZE = 4096 * 8;

protected:
	DrumKitLoader loader;
	DrumgizmoConfig config;

	AudioOutputEngine& oe;
	AudioInputEngine& ie;

	EventsDS events_ds;

	bool enable_resampling{true};

	std::map<std::string, AudioFile *> audiofiles;

	AudioCache audio_cache;
	DrumKit kit;
	InputProcessor input_processor;

	size_t framesize{0};
	bool freewheel{true};

	std::vector<event_t> events;
	Settings& settings;
	SettingsGetter settings_getter;

	Random rand;
	std::array<ZRWrapper, NUM_CHANNELS> zita;
	std::array<std::unique_ptr<sample_t>, NUM_CHANNELS> resampler_input_buffer;
	double ratio = 1.0;

};

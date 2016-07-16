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

#include <string>
#include <list>
#include <array>

#include "audiooutputengine.h"
#include "audioinputengine.h"
#include "events.h"
#include "audiofile.h"
#include "drumkit.h"
#include "drumkitloader.h"
#include "audiocache.h"
#include "mutex.h"
#include "chresampler.h"
#include "settings.h"
#include "inputprocessor.h"

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

	void getSamples(int ch, int pos, sample_t *s, size_t sz);

	//! Get the current engine latency in samples.
	std::size_t getLatency() const;

	int samplerate();
	void setSamplerate(int samplerate);

	void setFrameSize(size_t framesize);

	void setFreeWheel(bool freewheel);

	void setRandomSeed(unsigned int seed);

private:
	static constexpr int MAX_NUM_CHANNELS = 64;
	static constexpr int RESAMPLER_OUTPUT_BUFFER = 4096;
	static constexpr int RESAMPLER_INPUT_BUFFER = 64;

protected:
	DrumKitLoader loader;

	Mutex mutex;

	AudioOutputEngine& oe;
	AudioInputEngine& ie;

	std::list< Event* > activeevents[MAX_NUM_CHANNELS];

	Resamplers resamplers;
	sample_t resampler_output_buffer[MAX_NUM_CHANNELS][RESAMPLER_OUTPUT_BUFFER];
	sample_t resampler_input_buffer[MAX_NUM_CHANNELS][RESAMPLER_INPUT_BUFFER];

	std::map<std::string, AudioFile *> audiofiles;

	AudioCache audioCache;
	DrumKit kit;
	InputProcessor input_processor;

	size_t framesize;
	bool freewheel;

	std::vector<event_t> events;
	Settings& settings;
	Random rand;
};

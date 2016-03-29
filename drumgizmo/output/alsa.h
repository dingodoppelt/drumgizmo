/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            alsa.h
 *
 *  Do 21. Jan 16:48:32 CET 2016
 *  Copyright 2016 Christian Glöckner
 *  cgloeckner@freenet.de
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

// Use the newer ALSA API
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <audiotypes.h>
#include <alsa/asoundlib.h>

#include "audiooutputengine.h"

class AlsaOutputEngine
	: public AudioOutputEngine
{
public:
	AlsaOutputEngine();
	~AlsaOutputEngine();

	// based on AudioOutputEngine
	bool init(const Channels& chan) override;
	void setParm(const std::string& parm, const std::string& value) override;
	bool start() override;
	void stop() override;
	void pre(size_t nsamples) override;
	void run(int ch, sample_t* samples, size_t nsamples) override;
	void post(size_t nsamples) override;
	size_t samplerate() override;

private:
	snd_pcm_t* handle;
	snd_pcm_hw_params_t* params;
	std::vector<sample_t> data;
	size_t num_channels;

	std::string dev;
	unsigned int srate; // samplerate
	snd_pcm_uframes_t frames;
};

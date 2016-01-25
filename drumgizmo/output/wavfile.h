/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            wavfile.h
 *
 *  Mi 20. Jan 16:57:16 CET 2016
 *  Copyright 2016 Christian Glöckner
 *  cgloeckner@freenet.de
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#pragma once
#include <string>
#include <vector>

#include <sndfile.h>

#include "audiooutputengine.h"

class WavfileOutputEngine : public AudioOutputEngine
{
public:
	WavfileOutputEngine();
	~WavfileOutputEngine();

	// based on AudioOutputEngine
	bool init(Channels data) override;
	void setParm(std::string parm, std::string value) override;
	bool start() override;
	void stop() override;
	void pre(size_t nsamples) override;
	void run(int ch, sample_t *samples, size_t nsamples) override;
	void post(size_t nsamples) override;
	size_t samplerate() override;

private:
	SF_INFO info;
	std::vector<SNDFILE *> channels;
	size_t num_channels;

	std::string file;
};

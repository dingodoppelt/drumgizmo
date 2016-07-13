/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audiooutputengine.h
 *
 *  Thu Sep 16 10:27:01 CEST 2010
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

#include <cstddef>
#include <string>

#include <audiotypes.h>

#include "channel.h"

class AudioOutputEngine
{
public:
	virtual ~AudioOutputEngine() = default;

	virtual bool init(const Channels& channels) = 0;

	virtual void setParm(const std::string& parm, const std::string& value) = 0;

	virtual bool start() = 0;
	virtual void stop() = 0;

	virtual void pre(std::size_t nsamples) = 0;
	virtual void run(int ch, sample_t *samples, std::size_t nsamples) = 0;
	virtual void post(std::size_t nsamples) = 0;

	//! Overload this method to use internal buffer directly.
	virtual sample_t *getBuffer(int ch) const { return NULL; }

	//! Overload this method to force engine to use different buffer size.
	virtual std::size_t getBufferSize() const { return 1024; }

	virtual std::size_t getSamplerate() const { return 44100; }

	//! Overload this method to get notification of latency changes.
	//! \param latency The new latency in samples.
	virtual void onLatencyChange(std::size_t latency) {}
};

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            event.h
 *
 *  Sat Sep 18 22:02:16 CEST 2010
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

#include <map>
#include <stdio.h>
#include <string>
#include <mutex>

#include "audiofile.h"
#include "audio.h"
#include "audiocache.h"

typedef unsigned int timepos_t;

class Event
{
public:
	Event(channel_t channel, timepos_t offset = 0)
		: channel(channel), offset(offset)
	{
	}

	virtual ~Event()
	{
	}

	typedef enum
	{
		sample
	} type_t;

	virtual type_t getType() const = 0;

	channel_t channel;
	timepos_t offset; //< Global position (ie. not relative to buffer)
};

class EventSample
	: public Event
{
public:
	EventSample(channel_t c, float g, AudioFile* af,
	            const std::string& grp, std::size_t instrument_id)
		: Event(c)
		, cache_id(CACHE_NOID)
		, gain(g)
		, t(0)
		, file(af)
		, group(grp)
		, rampdown_count(-1)
		, ramp_length(0)
		, instrument_id(instrument_id)
	{
	}

	Event::type_t getType() const
	{
		return Event::sample;
	}

	bool rampdownInProgress() const
	{
		return rampdown_count != -1;
	}

	cacheid_t cache_id;
	sample_t* buffer;
	std::size_t buffer_size;
	std::size_t buffer_ptr{0}; //< Internal pointer into the current buffer
	std::size_t sample_size{0}; //< Total number of audio samples in this sample.

	float gain;
	unsigned int t; //< Internal sample position.
	AudioFile* file;
	std::string group;
	int rampdown_count;
	int ramp_length;
	std::size_t rampdown_offset{0};
	float scale{1.0f};
	std::size_t instrument_id;
};

class EventQueue
{
public:
	void post(Event* event, timepos_t time);
	Event* take(timepos_t time);
	bool hasEvent(timepos_t time);
	size_t getSize() const
	{
		return queue.size();
	}

private:
	std::multimap<timepos_t, Event*> queue;
	std::mutex mutex;
};

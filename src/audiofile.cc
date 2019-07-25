/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audiofile.cc
 *
 *  Tue Jul 22 17:14:11 CEST 2008
 *  Copyright 2008 Bent Bisballe Nyeng
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
 *
 *  Multichannel feature by John Hammen copyright 2014
 */
#include "audiofile.h"

#include <cassert>
#include <sndfile.h>

#include <config.h>

#include <hugin.hpp>

#include "channel.h"

AudioFile::AudioFile(const std::string& filename, std::size_t filechannel,
                     InstrumentChannel* instrument_channel)
	: filename(filename)
	, filechannel(filechannel)
	, magic{this}
	, instrument_channel(instrument_channel)
{
}

AudioFile::~AudioFile()
{
	magic = nullptr;
	unload();
}

bool AudioFile::isValid() const
{
	//assert(this == magic);
	return this == magic;
}

void AudioFile::unload()
{
	// Make sure we don't unload the object while loading it...
	std::lock_guard<std::mutex> guard(mutex);

	is_loaded = false;

	preloadedsize = 0;
	size = 0;
	delete[] data;
	data = nullptr;
}

#define BUFFER_SIZE 4096

void AudioFile::load(LogFunction logger, std::size_t sample_limit)
{
	// Make sure we don't unload the object while loading it...
	std::lock_guard<std::mutex> guard(mutex);

	if(this->data) // already loaded
	{
		return;
	}

	SF_INFO sf_info{};
	SNDFILE *fh = sf_open(filename.c_str(), SFM_READ, &sf_info);
	if(!fh)
	{
		ERR(audiofile,"SNDFILE Error (%s): %s\n",
		    filename.c_str(), sf_strerror(fh));
		if(logger)
		{
			logger(LogLevel::Warning, "Could not load '" + filename +
			       "': " + sf_strerror(fh));
		}
		return;
	}

	if(sf_info.channels < 1)
	{
		// This should never happen but lets check just in case.
		if(logger)
		{
			logger(LogLevel::Warning, "Could not load '" + filename +
			       "': no audio channels available.");
		}
		return;
	}

	std::size_t size = sf_info.frames;
	std::size_t preloadedsize = sf_info.frames;

	if(preloadedsize > sample_limit)
	{
		preloadedsize = sample_limit;
	}

	sample_t* data = new sample_t[preloadedsize];
	if(sf_info.channels == 1)
	{
		preloadedsize = sf_read_float(fh, data, preloadedsize);
	}
	else
	{
		// check filechannel exists
		if(filechannel >= (std::size_t)sf_info.channels)
		{
			if(logger)
			{
				logger(LogLevel::Warning, "Audio file '" + filename +
				       "' does no have " + std::to_string(filechannel + 1) + " channels.");
			}
			filechannel = sf_info.channels - 1;
		}

		sample_t buffer[BUFFER_SIZE];
		std::size_t frame_count = BUFFER_SIZE / sf_info.channels;
		std::size_t total_frames_read = 0;
		int frames_read;

		do
		{
	    frames_read = sf_readf_float(fh, buffer, frame_count);
	    for(int i = 0;
	        (i < frames_read) && (total_frames_read < sample_limit);
	        ++i)
	    {
		    data[total_frames_read++] = buffer[i * sf_info.channels + filechannel];
	    }
		}
		while( (frames_read > 0) &&
		       (total_frames_read < preloadedsize) &&
		       (total_frames_read < sample_limit) );

		// set data size to total bytes read
		preloadedsize = total_frames_read;
	}

	sf_close(fh);

	this->data = data;
	this->size = size;
	this->preloadedsize = preloadedsize;
	is_loaded = true;
}

bool AudioFile::isLoaded() const
{
	return is_loaded;
}

main_state_t AudioFile::mainState() const
{
	if(instrument_channel == nullptr)
	{
		DEBUG(audiofile, "no instrument_channel!");
		return main_state_t::unset;
	}

	return instrument_channel->main;
}

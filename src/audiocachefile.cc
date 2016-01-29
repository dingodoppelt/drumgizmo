/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            cacheaudiofile.cc
 *
 *  Thu Dec 24 12:17:58 CET 2015
 *  Copyright 2015 Bent Bisballe Nyeng
 *  deva@aasimon.org
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
#include "audiocachefile.h"

#include <assert.h>

#include <hugin.hpp>

#include <cstring>

#include "audiocache.h"

AudioCacheFile::AudioCacheFile(const std::string& filename,
                               std::vector<sample_t>& read_buffer)
	: filename(filename), read_buffer(read_buffer)
{
	std::memset(&sf_info, 0, sizeof(SF_INFO));

	fh = sf_open(filename.c_str(), SFM_READ, &sf_info);
	if(!fh)
	{
		ERR(audiofile,"SNDFILE Error (%s): %s\n",
		    filename.c_str(), sf_strerror(fh));
		return;
	}

	if(sf_info.frames == 0)
	{
		WARN(cache, "sf_info.frames == 0\n");
	}
}

AudioCacheFile::~AudioCacheFile()
{
	if(fh)
	{
		sf_close(fh);
		fh = nullptr;
	}
}

size_t AudioCacheFile::getSize() const
{
	return sf_info.frames;
}

const std::string& AudioCacheFile::getFilename() const
{
	return filename;
}

size_t AudioCacheFile::getChannelCount()
{
	return sf_info.channels;
}

void AudioCacheFile::readChunk(const CacheChannels& channels,
                               size_t pos, size_t num_samples)
{
	//assert(fh != nullptr); // File handle must never be nullptr
	if(!fh)
	{
		return;
	}

	if((int)pos > sf_info.frames)
	{
		WARN(cache, "pos (%d) > sf_info.frames (%d)\n",
		     (int)pos, (int)sf_info.frames);
		return;
	}

	sf_seek(fh, pos, SEEK_SET);

	size_t size = sf_info.frames - pos;
	if(size > num_samples)
	{
		size = num_samples;
	}

	if((size * sf_info.channels) > read_buffer.size())
	{
		read_buffer.resize(size * sf_info.channels);
	}

	size_t read_size = sf_readf_float(fh, read_buffer.data(), size);
	(void)read_size;

	for(auto it = channels.begin(); it != channels.end(); ++it)
	{
		size_t channel = it->channel;
		sample_t* data = it->samples;
		for (size_t i = 0; i < size; ++i)
		{
			data[i] = read_buffer[(i * sf_info.channels) + channel];
		}
	}

	for(auto it = channels.begin(); it != channels.end(); ++it)
	{
		*(it->ready) = true;
	}
}

AudioCacheFile& AudioCacheFiles::getFile(const std::string& filename)
{
	std::lock_guard<std::mutex> lock(mutex);

	auto it = audiofiles.find(filename);
	if(it == audiofiles.end())
	{
		it = audiofiles.emplace(std::piecewise_construct, std::make_tuple(filename),
		                        std::make_tuple(filename, std::ref(read_buffer))).first;
	}

	auto& cache_audio_file = it->second;

	// Increase ref count.
	++cache_audio_file.ref;

	return cache_audio_file;
}

void AudioCacheFiles::releaseFile(const std::string& filename)
{
	std::lock_guard<std::mutex> lock(mutex);

	auto it = audiofiles.find(filename);
	if(it == audiofiles.end())
	{
		assert(false); // This should never happen!
		return; // not open
	}

	auto& audiofile = it->second;

	assert(audiofile.ref); // If ref is not > 0 it shouldn't be in the map.

	--audiofile.ref;
	if(audiofile.ref == 0)
	{
		audiofiles.erase(it);
	}
}

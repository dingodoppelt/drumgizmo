/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            cacheaudiofile.h
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
#pragma once

#include <string>
#include <list>
#include <map>
#include <mutex>

#include <sndfile.h>

#include <audiotypes.h>

class CacheChannel {
public:
	size_t channel;
	sample_t* samples;
	size_t num_samples;
	volatile bool* ready;
};

using CacheChannels = std::list<CacheChannel>;

//! This class is used to encapsulate reading from a single file source.
//! The access is ref counted so that the file is only opened once and closed
//! when it is no longer required.
class AudioCacheFile {
	friend class AudioCacheFiles;
	friend class TestableAudioCacheFiles;
public:
	//! Create file handle for filename.
	AudioCacheFile(const std::string& filename);

	//! Closes file handle.
	~AudioCacheFile();

	//! Get sample count of the file connected with this cache object.
	size_t getSize() const;

	//! Get filename of the file connected with this cache object.
	const std::string& getFilename() const;

	//! Get number of channels in the file
	size_t getChannelCount();

	//! Read audio data from the file into the supplied channel caches.
	void readChunk(const CacheChannels& channels, size_t pos, size_t num_samples);

private:
	int ref{0};
	SNDFILE* fh{nullptr};
	SF_INFO sf_info;
	std::string filename;
};

class AudioCacheFiles {
public:
	//! Get the CacheAudioFile object corresponding to filename.
	//! If it does not exist it will be created.
	//! It's ref count will be increased.
	AudioCacheFile& getFile(const std::string& filename);

	//! Release the CacheAudioFile corresponding to filename.
	//! It's ref count will be decreased.
	//! If the ref count reaches 0 the object will be deleted.
	void releaseFile(const std::string& filename);

protected:
	std::map<std::string, AudioCacheFile*> audiofiles;
	std::mutex mutex;
};

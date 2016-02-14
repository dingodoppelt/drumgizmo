/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audiocache.h
 *
 *  Fri Apr 10 10:39:24 CEST 2015
 *  Copyright 2015 Jonas Suhr Christensen
 *  jsc@umbraculum.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
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
#include <vector>

#include "audiotypes.h"
#include "audiofile.h"

#include "audiocachefile.h"
#include "audiocacheidmanager.h"
#include "audiocacheeventhandler.h"

#define CHUNK_MULTIPLIER 16

class AudioCache {
public:
	AudioCache() = default;

	//! Destroy object and stop thread if needed.
	~AudioCache();

	//! Initialise cache manager and allocate needed resources
	//! This method starts the cache manager thread.
	//! This method blocks until the thread has been started.
	//! \param poolsize The maximum number of parellel events supported.
	void init(size_t poolsize);

	//! Stop thread and clean up resources.
	//! This method blocks until the thread has stopped.
	void deinit();

	//! Register new cache entry.
	//! Prepares an entry in the cache manager for future disk streaming.
	//! \param file A pointer to the file which is to be streamed from.
	//! \param initial_samples_needed The number of samples needed in the first
	//!  read that is not nessecarily of framesize. This is the number of samples
	//!  from the input event offset to the end of the frame in which it resides.
	//!  initial_samples_needed <= framesize.
	//! \param channel The channel to which the cache id will be bound.
	//! \param [out] new_id The newly created cache id.
	//! \return A pointer to the first buffer containing the
	//!  'initial_samples_needed' number of samples.
	sample_t* open(const AudioFile& file, size_t initial_samples_needed, int channel,
	               cacheid_t& new_id);

	//! Get next buffer.
	//! Returns the next buffer for reading based on cache id.
	//! This function will (if needed) schedule a new disk read to make sure that
	//! data is available in the next call to this method.
	//! \param id The cache id to read from.
	//! \param [out] size The size of the returned buffer.
	//! \return A pointer to the buffer.
	sample_t* next(cacheid_t id, size_t &size);

	//! Returns true iff the next chunk of the supplied id has been read from disk.
	bool isReady(cacheid_t id);

	//! Unregister cache entry.
	//! Close associated file handles and free associated buffers.
	//! \param id The cache id to close.
	void close(cacheid_t id);

	//! Set/get internal framesize used when iterating through cache buffers.
	void setFrameSize(size_t framesize);
	size_t frameSize() const;

	//! Control/get reader threaded mode.
	//! True means reading happening threaded, false means all reading done
	//! synchronious.
	void setAsyncMode(bool async);
	bool asyncMode() const;

	//! Return the number of chunks that were read too late.
	size_t getNumberOfUnderruns() const;

	//! Set underrun counter to 0.
	void resetNumberOfUnderruns();

private:
	size_t framesize{0};
	sample_t* nodata{nullptr};
	size_t number_of_underruns{0};

	AudioCacheIDManager id_manager;
	AudioCacheEventHandler event_handler{id_manager};
};

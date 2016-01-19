/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audiocacheidmanager.h
 *
 *  Tue Jan  5 10:59:37 CET 2016
 *  Copyright 2016 Bent Bisballe Nyeng
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

#include <stdlib.h>

#include <vector>

#include <audiotypes.h>

#include <mutex>

class AudioCacheFile;

#define CACHE_DUMMYID -2
#define CACHE_NOID -1

typedef int cacheid_t;

typedef struct {
	cacheid_t id{CACHE_NOID}; //< Current id of this cache_t. CACHE_NOID means not in use.

	AudioCacheFile* afile{nullptr};
	size_t channel{0};
	size_t pos{0}; //< File position
	volatile bool ready{false};
	sample_t* front{nullptr};
	sample_t* back{nullptr};
	size_t localpos{0}; //< Intra buffer (front) position.

	sample_t* preloaded_samples{nullptr}; // nullptr means preload buffer not active.
	size_t preloaded_samples_size{0};
} cache_t;

class AudioCacheIDManager {
	friend class AudioCacheEventHandler;
public:
	AudioCacheIDManager();
	~AudioCacheIDManager();

	void init(unsigned int capacity);

	// #thread safe
	// #hard real-time safe
	cache_t& getCache(cacheid_t id);

	// Thread safe
	// Real-time safe
	cacheid_t registerID(const cache_t& cache);

	// Thread safe
	// Real-time safe
	void releaseID(cacheid_t id);

protected:
	// For AudioCacheEventHandler
	void disableActive();
	std::vector<cacheid_t> getActiveIDs();

	std::mutex mutex;

	std::vector<cache_t> id2cache;
	std::vector<cacheid_t> availableids;
};

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audiocacheidmanager.cc
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
#include "audiocacheidmanager.h"

#include <limits>
#include <assert.h>

AudioCacheIDManager::~AudioCacheIDManager()
{
	assert(availableids.size() == id2cache.size()); // All ids should be released.
}

void AudioCacheIDManager::init(unsigned int capacity)
{
	std::lock_guard<std::mutex> guard(mutex);

	id2cache.resize(capacity);
	availableids.resize(capacity);
	for(size_t i = 0; i < capacity; ++i)
	{
		availableids[i] = i;
	}
}

cache_t& AudioCacheIDManager::getCache(cacheid_t id)
{
	std::lock_guard<std::mutex> guard(mutex);

	assert(id != CACHE_NOID);
	assert(id != CACHE_DUMMYID);
	assert(id >= 0);
	assert(id < (int)id2cache.size());
	assert(id2cache[id].id == id);

	return id2cache[id];
}

cacheid_t AudioCacheIDManager::registerID(const cache_t& cache)
{
	std::lock_guard<std::mutex> guard(mutex);

	cacheid_t id = CACHE_NOID;

	if(availableids.empty())
	{
		return CACHE_DUMMYID;
	}
	else
	{
		id = availableids.back();
		availableids.pop_back();
	}

	assert(id2cache[id].id == CACHE_NOID); // Make sure it is not already in use

	id2cache[id] = cache;
	id2cache[id].id = id;

	return id;
}

void AudioCacheIDManager::releaseID(cacheid_t id)
{
	std::lock_guard<std::mutex> guard(mutex);

	assert(id2cache[id].id != CACHE_NOID); // Test if it wasn't already released.

	id2cache[id].id = CACHE_NOID;

	availableids.push_back(id);
}

void AudioCacheIDManager::disableActive()
{
	// Run through all active cache_ts and disable them.
	for(auto& cache : id2cache)
	{
		if(cache.id != CACHE_NOID)
		{
			// Force use of nodata in all of the rest of the next() calls:
			cache.localpos = std::numeric_limits<size_t>::max();
			cache.ready = false;
		}
	}
}

std::vector<cacheid_t> AudioCacheIDManager::getActiveIDs()
{
	std::vector<cacheid_t> active_ids;

	for(auto& cache : id2cache)
	{
		if(cache.id != CACHE_NOID)
		{
			active_ids.push_back(cache.id);
		}
	}

	return active_ids;
}

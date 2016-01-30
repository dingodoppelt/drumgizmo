/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audiocacheeventhandler.cc
 *
 *  Sun Jan  3 19:57:55 CET 2016
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
#include "audiocacheeventhandler.h"

#include <assert.h>

#include <hugin.hpp>

#include "audiocachefile.h"
#include "audiocache.h"
#include "audiocacheidmanager.h"

enum class EventType {
	LoadNext,
	Close,
};

class CacheEvent {
public:
	EventType event_type;

	// For close event:
	cacheid_t id;

	// For load next event:
	size_t pos;
	AudioCacheFile* afile;
	CacheChannels channels;
};

AudioCacheEventHandler::AudioCacheEventHandler(AudioCacheIDManager& id_manager)
	: id_manager(id_manager)
{
}

AudioCacheEventHandler::~AudioCacheEventHandler()
{
	// Close all ids already enqueued to be closed.
	clearEvents();

	auto active_ids = id_manager.getActiveIDs();
	for(auto id : active_ids)
	{
		handleCloseCache(id);
	}
}

void AudioCacheEventHandler::start()
{
	if(running)
	{
		return;
	}

	running = true;
	run();
	sem_run.wait();
}

void AudioCacheEventHandler::stop()
{
	if(!running)
	{
		return;
	}

	running = false;

	sem.post();
	wait_stop();
}

void AudioCacheEventHandler::setThreaded(bool threaded)
{
	if(this->threaded == threaded)
	{
		return;
	}

	if(threaded && !running)
	{
		start();
	}
	else if(!threaded && running)
	{
		stop();
	}

	this->threaded = threaded;
}

bool AudioCacheEventHandler::isThreaded() const
{
	return threaded;
}

void AudioCacheEventHandler::lock()
{
	mutex.lock();
}

void AudioCacheEventHandler::unlock()
{
	mutex.unlock();
}

void AudioCacheEventHandler::pushLoadNextEvent(AudioCacheFile* afile,
                                               size_t channel,
                                               size_t pos, sample_t* buffer,
                                               volatile bool* ready)
{
	CacheEvent cache_event;
	cache_event.event_type = EventType::LoadNext;
	cache_event.pos = pos;
	cache_event.afile = afile;

	CacheChannel c;
	c.channel = channel;
	c.samples = buffer;

	*ready = false;
	c.ready = ready;

	cache_event.channels.insert(cache_event.channels.end(), c);

	pushEvent(cache_event);
}

void AudioCacheEventHandler::pushCloseEvent(cacheid_t id)
{
	CacheEvent cache_event;
	cache_event.event_type = EventType::Close;
	cache_event.id = id;

	pushEvent(cache_event);
}

void AudioCacheEventHandler::setChunkSize(size_t chunksize)
{
	DEBUG(cache, "%s\n", __PRETTY_FUNCTION__);

	// We should already locked when this method is called.
	//assert(!mutex.try_lock());

	if(this->chunksize == chunksize)
	{
		return;
	}

	DEBUG(cache, "setChunkSize 1\n");

	// Remove all events from event queue.
	clearEvents();

	DEBUG(cache, "setChunkSize 2\n");

	// Skip all active cacheids and make their buffers point at nodata.
	id_manager.disableActive();

	DEBUG(cache, "setChunkSize 3\n");

	this->chunksize = chunksize;
}

size_t AudioCacheEventHandler::chunkSize()
{
	return chunksize;
}

AudioCacheFile& AudioCacheEventHandler::openFile(const std::string& filename)
{
	std::lock_guard<std::mutex> lock(mutex);
	return files.getFile(filename);
}

void AudioCacheEventHandler::clearEvents()
{
	// Iterate all events ignoring load events and handling close events.
	for(auto& event : eventqueue)
	{
		if(event.event_type == EventType::Close)
		{
			handleCloseCache(event.id); // This method does not lock.
		}
	}

	eventqueue.clear();
}

void AudioCacheEventHandler::handleLoadNextEvent(CacheEvent& cache_event)
{
	cache_event.afile->readChunk(cache_event.channels, cache_event.pos,
	                             chunksize);
}

void AudioCacheEventHandler::handleCloseEvent(CacheEvent& cache_event)
{
	std::lock_guard<std::mutex> lock(mutex);
	handleCloseCache(cache_event.id);
}

void AudioCacheEventHandler::handleCloseCache(cacheid_t id)
{
	auto& cache = id_manager.getCache(id);

	files.releaseFile(cache.afile->getFilename());

	delete[] cache.front;
	delete[] cache.back;

	id_manager.releaseID(id);
}

void AudioCacheEventHandler::handleEvent(CacheEvent& cache_event)
{
	switch(cache_event.event_type)
	{
	case EventType::LoadNext:
		handleLoadNextEvent(cache_event);
		break;
	case EventType::Close:
		handleCloseEvent(cache_event);
		break;
	}
}

void AudioCacheEventHandler::thread_main()
{
	sem_run.post(); // Signal that the thread has been started

	while(running)
	{
		sem.wait();

		mutex.lock();
		if(eventqueue.empty())
		{
			mutex.unlock();
			continue;
		}

		CacheEvent cache_event = eventqueue.front();
		eventqueue.pop_front();
		mutex.unlock();

		// TODO: Skip event if cache_event.pos < cache.pos
		//if(!cache_event.active)
		//{
		//	continue;
		//}

		handleEvent(cache_event);
	}
}

void AudioCacheEventHandler::pushEvent(CacheEvent& cache_event)
{
	if(!threaded)
	{
		handleEvent(cache_event);
		return;
	}

	{
		std::lock_guard<std::mutex> lock(mutex);

		bool found = false;

		if(cache_event.event_type == EventType::LoadNext)
		{
			for(auto& queued_event : eventqueue)
			{
				if((queued_event.event_type == EventType::LoadNext) &&
				   (cache_event.afile->getFilename() ==
				    queued_event.afile->getFilename()) &&
				   (cache_event.pos == queued_event.pos))
				{
					// Append channel and buffer to the existing event.
					queued_event.channels.insert(queued_event.channels.end(),
					                             cache_event.channels.begin(),
					                             cache_event.channels.end());
					found = true;
					break;
				}
			}
		}

		if(!found)
		{
			// The event was not already on the list, create a new one.
			eventqueue.push_back(cache_event);
		}
	}

	sem.post();
}

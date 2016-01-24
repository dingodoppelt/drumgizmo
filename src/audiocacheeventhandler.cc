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

#include "audiocachefile.h"
#include "audiocache.h"
#include "audiocacheidmanager.h"

enum class EventType {
	LoadNext,
	Close,
};

class CacheEvent {
public:
	EventType eventType;

	// For close event:
	cacheid_t id;

	// For load next event:
	size_t pos;
	AudioCacheFile* afile;
	CacheChannels channels;
};

AudioCacheEventHandler::AudioCacheEventHandler(AudioCacheIDManager& idManager)
	// Hack to be able to forward declare CacheEvent:
	: eventqueue(new std::list<CacheEvent>())
	, idManager(idManager)
{
}

AudioCacheEventHandler::~AudioCacheEventHandler()
{
	// Close all ids already enqueued to be closed.
	clearEvents();

	auto activeIDs = idManager.getActiveIDs();
	for(auto id : activeIDs)
	{
		handleCloseCache(id);
	}

	// Hack to be able to forward declare CacheEvent:
	delete eventqueue;
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
	if(threaded)
	{
		sem.post();
		wait_stop();
	}
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

	if(!threaded && running)
	{
		stop();
	}

	this->threaded = threaded;
}

bool AudioCacheEventHandler::getThreaded() const
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
	CacheEvent e;
	e.eventType = EventType::LoadNext;
	e.pos = pos;
	e.afile = afile;

	CacheChannel c;
	c.channel = channel;
	c.samples = buffer;

	*ready = false;
	c.ready = ready;

	e.channels.insert(e.channels.end(), c);

	pushEvent(e);
}

void AudioCacheEventHandler::pushCloseEvent(cacheid_t id)
{
	CacheEvent e;
	e.eventType = EventType::Close;
	e.id = id;

	pushEvent(e);
}

void AudioCacheEventHandler::setChunkSize(size_t chunksize)
{
	// We should already locked when this method is called.
	assert(!mutex.try_lock());

	if(this->chunksize == chunksize)
	{
		return;
	}

	// Remove all events from event queue.
	clearEvents();

	// Skip all active cacheids and make their buffers point at nodata.
	idManager.disableActive();

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
	for(auto& event : *eventqueue)
	{
		if(event.eventType == EventType::Close)
		{
			handleCloseCache(event.id); // This method does not lock.
		}
	}

	eventqueue->clear();
}

void AudioCacheEventHandler::handleLoadNextEvent(CacheEvent& event)
{
	event.afile->readChunk(event.channels, event.pos, chunksize);
}

void AudioCacheEventHandler::handleCloseEvent(CacheEvent& e)
{
	std::lock_guard<std::mutex> lock(mutex);
	handleCloseCache(e.id);
}

void AudioCacheEventHandler::handleCloseCache(cacheid_t cacheid)
{
	auto& cache = idManager.getCache(cacheid);

	files.releaseFile(cache.afile->getFilename());

	delete[] cache.front;
	delete[] cache.back;

	idManager.releaseID(cacheid);
}

void AudioCacheEventHandler::handleEvent(CacheEvent& e)
{
	switch(e.eventType)
	{
	case EventType::LoadNext:
		handleLoadNextEvent(e);
		break;
	case EventType::Close:
		handleCloseEvent(e);
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
		if(eventqueue->empty())
		{
			mutex.unlock();
			continue;
		}

		CacheEvent e = eventqueue->front();
		eventqueue->pop_front();
		mutex.unlock();

		// TODO: Skip event if e.pos < cache.pos
		//if(!e.active)
		//{
		//	continue;
		//}

		handleEvent(e);
	}
}

void AudioCacheEventHandler::pushEvent(CacheEvent& event)
{
	if(!threaded)
	{
		handleEvent(event);
		return;
	}

	{
		std::lock_guard<std::mutex> lock(mutex);

		bool found = false;

		if(event.eventType == EventType::LoadNext)
		{
			for(auto& queuedEvent : *eventqueue)
			{
				if((queuedEvent.eventType == EventType::LoadNext) &&
				   (event.afile->getFilename() == queuedEvent.afile->getFilename()) &&
				   (event.pos == queuedEvent.pos))
				{
					// Append channel and buffer to the existing event.
					queuedEvent.channels.insert(queuedEvent.channels.end(),
					                            event.channels.begin(),
					                            event.channels.end());
					found = true;
					break;
				}
			}
		}

		if(!found)
		{
			// The event was not already on the list, create a new one.
			eventqueue->push_back(event);
		}
	}

	sem.post();
}

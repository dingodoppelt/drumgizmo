/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            cachemanager.cc
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
#include "cachemanager.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <hugin.hpp>

#include "cacheaudiofile.h"

#define CHUNKSIZE(x) (x * CHUNK_MULTIPLIER)


CacheManager::CacheManager()
	: framesize(0)
	, nodata(nullptr)
{
}

CacheManager::~CacheManager()
{
	deinit();
	delete[] nodata;
}

void CacheManager::init(size_t poolsize, bool threaded)
{
	this->threaded = threaded;

	id2cache.resize(poolsize);
	for(size_t i = 0; i < poolsize; ++i)
	{
		availableids.push_back(i);
	}

	running = true;
	if(threaded)
	{
		run();
		sem_run.wait();
	}
}

void CacheManager::deinit()
{
	if(!running) return;
	running = false;
	if(threaded)
	{
		sem.post();
		wait_stop();
	}
}

// Invariant: initial_samples_needed < preloaded audio data
sample_t *CacheManager::open(AudioFile *file, size_t initial_samples_needed,
                             int channel, cacheid_t &id)
{
	if(!file->isValid())
	{
		// File preload not yet ready - skip this sample.
		id = CACHE_DUMMYID;
		assert(nodata);
		return nodata;
	}

	{
		MutexAutolock l(m_ids);
		if(availableids.empty())
		{
			id = CACHE_DUMMYID;
		}
		else
		{
			id = availableids.front();
			availableids.pop_front();
		}
	}

	if(id == CACHE_DUMMYID)
	{
		assert(nodata);
		return nodata;
	}

	CacheAudioFile& afile = files.getAudioFile(file->filename);

	cache_t c;
	c.afile = &afile;
	c.channel = channel;

	// next call to 'next' will read from this point.
	c.localpos = initial_samples_needed;

	c.front = nullptr; // This is allocated when needed.
	c.back = nullptr; // This is allocated when needed.

	// cropped_size is the preload chunk size cropped to sample length.
	size_t cropped_size = file->preloadedsize - c.localpos;
	cropped_size /= framesize;
	cropped_size *= framesize;
	cropped_size += initial_samples_needed;

	if(file->preloadedsize == file->size)
	{
		// We have preloaded the entire file, so use it.
		cropped_size = file->preloadedsize;
	}

	c.preloaded_samples = file->data;
	c.preloaded_samples_size = cropped_size;

	// next read from disk will read from this point.
	c.pos = cropped_size;//c.preloaded_samples_size;

	{
		MutexAutolock l(m_ids);
		id2cache[id] = c;
	}
	// NOTE: Below this point we can no longer write to 'c'.

	// Only load next buffer if there are more data in the file to be loaded...
	if(c.pos < file->size)
	{
		cache_t& c = id2cache[id]; // Create new writeable 'c'.

		if(c.back == nullptr)
		{
			c.back = new sample_t[CHUNKSIZE(framesize)];
		}

		cevent_t e =
			createLoadNextEvent(c.afile, c.channel, c.pos, c.back, &c.ready);
		pushEvent(e);
	}

	return c.preloaded_samples; // return preloaded data
}

sample_t *CacheManager::next(cacheid_t id, size_t &size)
{
	size = framesize;

	if(id == CACHE_DUMMYID)
	{
		assert(nodata);
		return nodata;
	}

	cache_t& c = id2cache[id];

	if(c.preloaded_samples)
	{

		// We are playing from memory:
		if(c.localpos < c.preloaded_samples_size)
		{
			sample_t *s = c.preloaded_samples + c.localpos;
			c.localpos += framesize;
			return s;
		}

		c.preloaded_samples = nullptr; // Start using samples from disk.

	}
	else
	{

		// We are playing from cache:
		if(c.localpos < CHUNKSIZE(framesize))
		{
			sample_t *s = c.front + c.localpos;
			c.localpos += framesize;
			return s;
		}
	}

	if(!c.ready)
	{
		//printf("#%d: NOT READY!\n", id); // TODO: Count and show in UI?
		return nodata;
	}

	// Swap buffers
	std::swap(c.front, c.back);

	// Next time we go here we have already read the first frame.
	c.localpos = framesize;

	c.pos += CHUNKSIZE(framesize);

	if(c.pos < c.afile->getSize())
	{
		if(c.back == nullptr)
		{
			c.back = new sample_t[CHUNKSIZE(framesize)];
		}

		cevent_t e = createLoadNextEvent(c.afile, c.channel, c.pos, c.back, &c.ready);
		pushEvent(e);
	}

	if(!c.front)
	{
		printf("We shouldn't get here... ever!\n");
		assert(false);
		return nodata;
	}

	return c.front;
}

void CacheManager::close(cacheid_t id)
{
	if(id == CACHE_DUMMYID)
	{
		return;
	}

	cevent_t e = createCloseEvent(id);
	pushEvent(e);
}

void CacheManager::setFrameSize(size_t framesize)
{
	if(framesize > this->framesize)
	{
		delete[] nodata;
		nodata = new sample_t[framesize];

		for(size_t i = 0; i < framesize; ++i)
		{
			nodata[i] = 0;
		}
	}

	this->framesize = framesize;
}

void CacheManager::setAsyncMode(bool async)
{
	// TODO: Clean out read queue.
	// TODO: Block until reader thread is idle, otherwise we might screw up the
	// buffers...?
	threaded = async;
}

void CacheManager::handleLoadNextEvent(cevent_t &event)
{
	event.afile->readChunk(event.channels, event.pos, CHUNKSIZE(framesize));
}

void CacheManager::handleCloseEvent(cevent_t &e)
{
	handleCloseCache(e.id);
}

void CacheManager::handleCloseCache(cacheid_t& cacheid)
{
	cache_t& cache = id2cache[cacheid];
	{
		MutexAutolock l(m_events);

		files.release(cache.afile->getFilename());
	}

	delete[] cache.front;
	delete[] cache.back;

	{
		MutexAutolock l(m_ids);
		availableids.push_back(cacheid);
	}
}

void CacheManager::handleEvent(cevent_t &e)
{
	switch(e.cmd)
	{
	case LOADNEXT:
		handleLoadNextEvent(e);
		break;
	case CLOSE:
		handleCloseEvent(e);
		break;
	}
}

void CacheManager::thread_main()
{
	sem_run.post(); // Signal that the thread has been started

	while(running)
	{
		sem.wait();

		m_events.lock();
		if(eventqueue.empty())
		{
			m_events.unlock();
			continue;
		}

		cevent_t e = eventqueue.front();
		eventqueue.pop_front();
		m_events.unlock();

		// TODO: Skip event if e.pos < cache.pos
		// if(!e.active) continue;

		handleEvent(e);
	}
}

void CacheManager::pushEvent(cevent_t& e)
{
	if(!threaded)
	{
		handleEvent(e);
		return;
	}

	{
		MutexAutolock l(m_events);

		bool found = false;

		if(e.cmd == LOADNEXT)
		{
			for(auto it = eventqueue.begin(); it != eventqueue.end(); ++it)
			{
				auto& event = *it;
				if((event.cmd == LOADNEXT) &&
				   (e.afile->getFilename() == event.afile->getFilename()) &&
				   (e.pos == event.pos))
				{
					// Append channel and buffer to the existing event.
					event.channels.insert(event.channels.end(), e.channels.begin(),
					                      e.channels.end());
					found = true;
					break;
				}
			}
		}

		if(!found)
		{
			// The event was not already on the list, create a new one.
			eventqueue.push_back(e);
		}
	}

	sem.post();
}

CacheManager::cevent_t
CacheManager::createLoadNextEvent(CacheAudioFile *afile, size_t channel,
                                  size_t pos, sample_t* buffer,
                                  volatile bool* ready)
{
	cevent_t e;
	e.cmd = LOADNEXT;
	e.pos = pos;
	e.afile = afile;

	CacheChannel c;
	c.channel = channel;
	c.samples = buffer;

	*ready = false;
	c.ready = ready;

	e.channels.insert(e.channels.end(), c);

	return e;
}

CacheManager::cevent_t
CacheManager::createCloseEvent(cacheid_t id)
{
	cevent_t e;
	e.cmd = CLOSE;
	e.id = id;
	return e;
}

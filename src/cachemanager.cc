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

#include <stdio.h>

#define CHUNKSIZE 256

sample_t nodata[CHUNKSIZE];

CacheManager::CacheManager()
{
  for(int i = 0; i < CHUNKSIZE; i++) {
    nodata[i] = 0;
  }
}

CacheManager::~CacheManager()
{
  deinit();
}

void CacheManager::init(int poolsize)
{
  id2cache.resize(poolsize);
  for(int i = 0; i < poolsize; i++) {
    availableids.push_back(i);
  }

  running = true;
  run();
}

void CacheManager::deinit()
{
  if(!running) return;
  running = false;
  wait_stop();
}

// Invariant: initial_samples_needed < preloaded audio data 
sample_t *CacheManager::open(AudioFile *file, size_t initial_samples_needed, int channel, cacheid_t &id) 
{
  m_ids.lock();
  if(availableids.empty()) {
    id = CACHE_DUMMYID;
  }
  else {
    id = availableids.front();
    availableids.pop_front();
  }
  m_ids.unlock();

  if(id == CACHE_DUMMYID) {
    return nodata;
  }

  if(initial_samples_needed < file->size) {
    cache_t c;
    c.file = file;
    c.channel = channel;
    c.pos = initial_samples_needed;
  
    // Allocate buffers
    // Increase audio ref count

    {
    MutexAutolock l(m_ids);
    id2cache[id] = c;
    }
 
    event_t e = createLoadNextEvent(id, c.pos, LOADNEXT);
    pushEvent(e);
  }

  return file->data;
}

void CacheManager::close(cacheid_t id)
{
  if(id == CACHE_DUMMYID) return;

  {
//    event_t e = createEvent(id, CLEAN);
//    MutexAutolock l(m_events); 
//    eventqueue.push_front(e);
  }
  
  {
    MutexAutolock l(m_ids);
    availableids.push_back(id);
  }
  // Clean cache_t mapped to event
  // Decrement audiofile ref count
}

const CacheManager::cache_t CacheManager::getNextCache(cacheid_t id)
{
  MutexAutolock l(m_caches);
  cache_t c = id2cache[id];
  id2cache[id].pos += CHUNKSIZE;
  return c;
}

sample_t *CacheManager::next(cacheid_t id, size_t &size) 
{
  size = CHUNKSIZE;

  if(id == CACHE_DUMMYID) {
    return nodata;
  }

  const cache_t c = getNextCache(id);

  // If more is left of file 
  if(c.pos < c.file->size) {
    event_t e = createLoadNextEvent(id, c.pos + CHUNKSIZE, LOADNEXT); 
    pushEvent(e);
  } 

  sample_t *s = c.file->data + (c.pos - size);
  return s;
}

void CacheManager::loadNext(cacheid_t id) 
{
  MutexAutolock l(m_caches);
  cache_t c = id2cache[id];
  id2cache[id] = c;
}

void CacheManager::thread_main()
{
  while(running) {
    sem.wait();

    m_events.lock();
    if(!eventqueue.empty()) {
      event_t e = eventqueue.front();
      eventqueue.pop_front();
      m_events.unlock();

      // TODO: Skip event if e.pos < cache.pos 
      if(!e.active) continue;

      switch(e.cmd) {  
        case LOADNEXT:
          loadNext(e.id);
          break;
        case CLEAN:
          break;
      }      
    }
    else {
      m_events.unlock();
    }
  }
}

void CacheManager::pushEvent(event_t e)
{
  // Check that if event should be merged (Maybe by event queue (ie. push in front).
  {
  MutexAutolock l(m_events);
  eventqueue.push_back(e);
  }
  sem.post();
}

CacheManager::event_t CacheManager::createLoadNextEvent(cacheid_t id, size_t pos, cmd_t cmd)
{
  event_t e;
  e.active = true;
  e.id = id;
  e.cmd = cmd;
  e.pos = pos;
  return e; 
}

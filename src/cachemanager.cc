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

#define FRAMESIZE 256
#define CHUNKSIZE FRAMESIZE*8

sample_t nodata[FRAMESIZE];

static std::vector<size_t> localcachepos;
static std::vector<sample_t*> localcache;

CacheManager::CacheManager()
{

}

CacheManager::~CacheManager()
{
  deinit();
}

void CacheManager::init(size_t poolsize)
{
  for(size_t i = 0; i < FRAMESIZE; i++) {
    nodata[i] = 0;
  }

  id2cache.resize(poolsize);
  for(size_t i = 0; i < poolsize; i++) {
    availableids.push_back(i);
  }

  localcachepos.resize(poolsize);
  for(size_t i = 0; i < poolsize; i++) {
    localcachepos[i] = 0;
  }

  localcache.resize(poolsize);
  for(size_t i = 0; i < poolsize; i++) {
    localcache[i] = NULL;
  } 
  
  running = true;
  run();

  // TODO: Add semaphore
}

void CacheManager::deinit()
{
  if(!running) return;
  running = false;
  wait_stop();
}

// Invariant: initial_samples_needed < preloaded audio data 
// Proposal: preloaded > 2 x CHUNKSIZE? So that we can fill c.front immediatly on open
sample_t *CacheManager::open(AudioFile *file, size_t initial_samples_needed, int channel, cacheid_t &id) 
{
  {
    MutexAutolock l(m_ids);
    if(availableids.empty()) {
      id = CACHE_DUMMYID;
    } else {
      id = availableids.front();
      availableids.pop_front();
    }
  }

  if(id == CACHE_DUMMYID) {
    return nodata;
  }

  cache_t c;
  c.file = file;
  c.channel = channel;
  c.pos = initial_samples_needed;
  c.front = file->data + initial_samples_needed;
  c.back = file->data;
  // Allocate buffers
  // Increase audio ref count

  {
    MutexAutolock l(m_ids);
    id2cache[id] = c;
  }

  localcachepos[id] = 0;
  localcache[id] = c.front;

  if(initial_samples_needed < file->size) {
    event_t e = createLoadNextEvent(id, c.pos, &id2cache[id].back);
    pushEvent(e);
  }

  return file->data; // preloaded data
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

CacheManager::cache_t CacheManager::getNextCache(cacheid_t id)
{
  MutexAutolock l(m_caches);
  cache_t &c = id2cache[id];

  sample_t *tmp = c.front;
  c.front = c.back;
  c.back = tmp;

  c.pos += CHUNKSIZE;
  
  return c;
}

sample_t *CacheManager::next(cacheid_t id, size_t &size) 
{
  size = FRAMESIZE;

  if(id == CACHE_DUMMYID) {
    return nodata;
  }

  if(localcachepos[id] < CHUNKSIZE) {
    localcachepos[id] += size;
    return localcache[id] + localcachepos[id];
  }

  cache_t c = getNextCache(id);
  
  localcachepos[id] = 0;
  localcache[id] = c.front;

  if(c.pos < c.file->size) {
    event_t e = createLoadNextEvent(id, c.pos + CHUNKSIZE, &id2cache[id].back); 
    pushEvent(e);
  } 

//  sample_t *s = c.file->data + (c.pos - size);
  return c.front;
//  return s;
}

void CacheManager::loadNext(event_t &e) 
{
  cache_t c = id2cache[e.id];
  *e.fillbuffer = c.file->data + e.pos;
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
//      if(!e.active) continue;

      switch(e.cmd) {  
        case LOADNEXT:
          loadNext(e);
          break;
//        case CLEAN:
//          break;
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

CacheManager::event_t CacheManager::createLoadNextEvent(cacheid_t id, size_t pos, sample_t** fillbuffer)
{
  event_t e;
  e.active = true;
  e.id = id;
  e.cmd = LOADNEXT;
  e.pos = pos;
  e.fillbuffer = fillbuffer;
  return e; 
}

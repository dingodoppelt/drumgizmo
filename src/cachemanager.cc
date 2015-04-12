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

#include <sndfile.h>

#include <hugin.hpp>

#define FRAMESIZE 256
#define CHUNKSIZE FRAMESIZE*100

static sample_t nodata[FRAMESIZE];

#define BUFFER_SIZE FRAMESIZE
static size_t readChunk(std::string filename, int filechannel, size_t from, size_t num_samples, sample_t* buf)
{
  SF_INFO sf_info;
  SNDFILE *fh = sf_open(filename.c_str(), SFM_READ, &sf_info);
  if(!fh) {
    ERR(audiofile,"SNDFILE Error (%s): %s\n",
        filename.c_str(), sf_strerror(fh));
    return 0;
  }

  sf_seek(fh, from, SEEK_SET);

  size_t size = num_samples;
  sample_t* data = buf; 
  if(sf_info.channels == 1) {
    size = sf_read_float(fh, data, size);
  }
  else {
    // check filechannel exists
    if(filechannel >= sf_info.channels) {
        filechannel = sf_info.channels - 1;
    }
    sample_t buffer[BUFFER_SIZE];
    int readsize = BUFFER_SIZE / sf_info.channels;
    int totalread = 0;
    int read;
    do {
      read = sf_readf_float(fh, buffer, readsize);
      for (int i = 0; i < read; i++) {
        data[totalread++] = buffer[i * sf_info.channels + filechannel];
      }
    } while(read > 0 && totalread < (int)size);
    // set data size to total bytes read
    size = totalread;
  }

  sf_close(fh);

  return size;
}

CacheManager::CacheManager() {}

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
  c.localpos = 0;
  c.front = new sample_t[CHUNKSIZE];
  c.back = new sample_t[CHUNKSIZE];

  memcpy(c.front, c.file->data + c.pos, CHUNKSIZE * sizeof(sample_t));

  // Increase audio ref count

  {
    MutexAutolock l(m_ids);
    id2cache[id] = c;
  }

  if(initial_samples_needed < file->size) {
    cevent_t e =
      createLoadNextEvent(c.file, c.channel, c.pos + CHUNKSIZE, c.back);
    pushEvent(e);
  }

  return file->data; // preloaded data
}

void CacheManager::close(cacheid_t id)
{
  if(id == CACHE_DUMMYID) return;

  cevent_t e = createCloseEvent(id);
  pushEvent(e);

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

sample_t *CacheManager::next(cacheid_t id, size_t &size) 
{
  size = FRAMESIZE;

  if(id == CACHE_DUMMYID) {
    return nodata;
  }

  cache_t& c = id2cache[id];
  if(c.localpos < CHUNKSIZE) {
    c.localpos += size;
    return c.front + c.localpos;
  }

  // Swap buffers
  sample_t *tmp = c.front;
  c.front = c.back;
  c.back = tmp;

  c.localpos = 0;

  c.pos += CHUNKSIZE;
  
  if(c.pos < c.file->size) {
    cevent_t e = createLoadNextEvent(c.file, c.channel, c.pos, c.back);
    pushEvent(e);
  } 

  return c.front;
}

void CacheManager::handleLoadNextEvent(cevent_t &e) 
{
//  memcpy(e.buffer, e.file->data + e.pos, CHUNKSIZE * sizeof(sample_t));
  readChunk(e.file->filename, e.channel, e.pos, CHUNKSIZE, e.buffer);
}

void CacheManager::handleCloseEvent(cevent_t &e) 
{
  cache_t& c = id2cache[e.id];
  delete[] c.front;
  delete[] c.back;
  // TODO: Count down ref coutner on c.file and close it if 0.
}

void CacheManager::thread_main()
{
  while(running) {
    sem.wait();

    m_events.lock();
    if(!eventqueue.empty()) {
      cevent_t e = eventqueue.front();
      eventqueue.pop_front();
      m_events.unlock();

      // TODO: Skip event if e.pos < cache.pos 
//      if(!e.active) continue;

      switch(e.cmd) {  
        case LOADNEXT:
          handleLoadNextEvent(e);
          break;
        case CLOSE:
          handleCloseEvent(e);
          break;
      }      
    } else {
      m_events.unlock();
    }
  }
}

void CacheManager::pushEvent(cevent_t e)
{
  // Check that if event should be merged (Maybe by event queue (ie. push in front).
  {
    MutexAutolock l(m_events);
    eventqueue.push_back(e);
  }
  sem.post();
}

CacheManager::cevent_t
CacheManager::createLoadNextEvent(AudioFile *file, size_t channel, size_t pos,
                                  sample_t* buffer)
{
  cevent_t e;
  e.cmd = LOADNEXT;
  e.pos = pos;
  e.buffer = buffer;
  e.file = file;
  e.channel = channel;
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

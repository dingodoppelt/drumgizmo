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

static sample_t nodata[FRAMESIZE];

#define	BUFFER_SIZE	4092

static size_t readChunk(std::string filename, int filechannel, size_t pos,
                        size_t num_samples, sample_t* buf)
{
  SF_INFO sf_info;
  SNDFILE *fh = sf_open(filename.c_str(), SFM_READ, &sf_info);
  if(!fh) {
    ERR(audiofile,"SNDFILE Error (%s): %s\n",
        filename.c_str(), sf_strerror(fh));
    return 0;
  }

  if(pos > sf_info.frames) {
    return 0;
  }

  sf_seek(fh, pos, SEEK_SET);

  size_t size = sf_info.frames - pos;
  if(size > num_samples) size = num_samples;

  sample_t* data = buf;
  if(sf_info.channels == 1) {
    size = sf_readf_float(fh, data, size);
  } else {
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
      for (int i = 0; i < read && totalread < (int)size; i++) {
        data[totalread++] = buffer[i * sf_info.channels + filechannel];
      }
    } while(read > 0 && totalread < (int)size && totalread < sf_info.frames);
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

void CacheManager::init(size_t poolsize, bool threaded)
{
  this->threaded = threaded;

  for(size_t i = 0; i < FRAMESIZE; i++) {
    nodata[i] = 0;
  }

  id2cache.resize(poolsize);
  for(size_t i = 0; i < poolsize; i++) {
    availableids.push_back(i);
  }

  running = true;
  if(threaded) {
    run();
    sem_run.wait();
  }
}

void CacheManager::deinit()
{
  if(!running) return;
  running = false;
  if(threaded) {
    sem.post();
    wait_stop();
  }
}

// Invariant: initial_samples_needed < preloaded audio data 
// Proposal: preloaded > 2 x CHUNKSIZE? So that we can fill c.front immediatly on open
sample_t *CacheManager::open(AudioFile *file, size_t initial_samples_needed,
                             int channel, cacheid_t &id)
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

  size_t size = CHUNKSIZE;
  if(size > (file->preloadedsize - c.pos)) size = (file->preloadedsize - c.pos);
  memcpy(c.front, c.file->data + c.pos, size * sizeof(sample_t));
  c.ready = false;
  c.pos += size;

  // Increase audio ref count

  {
    MutexAutolock l(m_ids);
    id2cache[id] = c;
  }

  // Only load next buffer if there are more data in the file to be loaded...
  if(c.pos < file->size) {
    cevent_t e =
      createLoadNextEvent(c.file, c.channel, c.pos, c.back);
    e.ready = &id2cache[id].ready;
    pushEvent(e);
  }

  return file->data; // preloaded data
}

sample_t *CacheManager::next(cacheid_t id, size_t &size) 
{
  size = FRAMESIZE;

  if(id == CACHE_DUMMYID) {
    return nodata;
  }

  cache_t& c = id2cache[id];
  if(c.localpos < CHUNKSIZE) {
    sample_t *s = c.front + c.localpos;
    c.localpos += size;
    return s;
  }

  if(!c.ready) {
    printf("#%d: NOT READY!\n", id); // TODO: Count and show in UI?
  }

  // Swap buffers
  sample_t *tmp = c.front;
  c.front = c.back;
  c.back = tmp;

  c.localpos = size; // Next time we go here we have already read the first frame.

  c.pos += CHUNKSIZE;
  
  if(c.pos < c.file->size) {
    cevent_t e = createLoadNextEvent(c.file, c.channel, c.pos, c.back);
    c.ready = false;
    e.ready = &c.ready;
    pushEvent(e);
  } 

  return c.front;
}

void CacheManager::close(cacheid_t id)
{
  if(id == CACHE_DUMMYID) {
    return;
  }

  cevent_t e = createCloseEvent(id);
  pushEvent(e);
}

void CacheManager::handleLoadNextEvent(cevent_t &e) 
{
#if 0 // memcpy
  size_t size = CHUNKSIZE;
  if(size > (e.file->size - e.pos)) {
    size = (e.file->size - e.pos);
  }
  memcpy(e.buffer, e.file->data + e.pos, size * sizeof(sample_t));
#elif 1 // diskread
  //memset(e.buffer, 0, CHUNKSIZE * sizeof(sample_t));
  readChunk(e.file->filename, e.channel, e.pos, CHUNKSIZE, e.buffer);
#endif
  *e.ready = true;
}

void CacheManager::handleCloseEvent(cevent_t &e) 
{
  cache_t& c = id2cache[e.id];
  delete[] c.front;
  delete[] c.back;

  {
    MutexAutolock l(m_ids);
    availableids.push_back(e.id);
  }

  // TODO: Count down ref coutner on c.file and close it if 0.
}


void CacheManager::handleEvent(cevent_t &e)
{
  switch(e.cmd) {
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

  while(running) {
    sem.wait();

    m_events.lock();
    if(eventqueue.empty()) {
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

void CacheManager::pushEvent(cevent_t e)
{
  if(!threaded) {
    handleEvent(e);
    return;
  }

  // Check that if event should be merged (Maybe by event queue (ie. push
  // in front).
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

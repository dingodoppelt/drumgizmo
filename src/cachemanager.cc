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

#include <sndfile.h>

#include <hugin.hpp>

#define CHUNKSIZE(x) (x * CHUNK_MULTIPLIER)

class AFile {
public:
  AFile(std::string filename)
    : ref(0)
    , filename(filename)
  {
    fh = sf_open(filename.c_str(), SFM_READ, &sf_info);
    if(!fh) {
      ERR(audiofile,"SNDFILE Error (%s): %s\n",
          filename.c_str(), sf_strerror(fh));
      //return 0;
    }
  }

  ~AFile()
  {
    sf_close(fh);
    fh = NULL;
  }

  int ref;
  SNDFILE* fh;
  SF_INFO sf_info;
  std::string filename;
};

static void readChunk(AFile* file, int filechannel, size_t pos,
                      size_t num_samples, sample_t* buf)
{
  SNDFILE* fh = file->fh;
  SF_INFO& sf_info = file->sf_info;

  if(fh == NULL) {
    printf("File handle is null.\n");
    return;
  }

  if(pos > sf_info.frames) {
    printf("pos > sf_info.frames\n");
    return;
  }

  sf_seek(fh, pos, SEEK_SET);

  size_t size = sf_info.frames - pos;
  if(size > num_samples) size = num_samples;

  static sample_t *read_buffer = NULL;
  static size_t read_buffer_size = 0;

  if((size * sf_info.channels) > read_buffer_size) {
    delete[] read_buffer;
    read_buffer_size = size * sf_info.channels;
    read_buffer = new sample_t[read_buffer_size];
    // TODO: This buffer is never free'd on app shutdown.
  }

  size_t read_size = sf_readf_float(fh, read_buffer, size);
  (void)read_size;

  size_t channel = filechannel;
  sample_t *data = buf;
  for (size_t i = 0; i < size; i++) {
    data[i] = read_buffer[(i * sf_info.channels) + channel];
  }
}

CacheManager::CacheManager()
  : framesize(0)
  , nodata(NULL)
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
sample_t *CacheManager::open(AudioFile *file, size_t initial_samples_needed,
                             int channel, cacheid_t &id)
{
  if(!file->isValid()) {
    // File preload not yet ready - skip this sample.
    id = CACHE_DUMMYID;
    assert(nodata);
    return nodata;
  }

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
    assert(nodata);
    return nodata;
  }

  AFile *afile = NULL;
  if(files.find(file->filename) == files.end()) {
    afile = new AFile(file->filename);
    files[file->filename] = afile;
  } else {
    afile = files[file->filename];
  }

  // Increase ref count.
  afile->ref++;

  cache_t c;
  c.file = afile;
  c.channel = channel;

  // next call to 'next' will read from this point.
  c.localpos = initial_samples_needed;

  c.front = NULL; // This is allocated when needed.
  c.back = NULL; // This is allocated when needed.

  // cropped_size is the preload chunk size cropped to sample length.
  size_t cropped_size = file->preloadedsize - c.localpos;
  cropped_size /= framesize;
  cropped_size *= framesize;
  cropped_size += initial_samples_needed;

  if(file->preloadedsize == file->size) {
    // We have preloaded the entire file, so use it.
    cropped_size = file->preloadedsize;
  }

  c.preloaded_samples = file->data;
  c.preloaded_samples_size = cropped_size;

  c.ready = false;

  // next read from disk will read from this point.
  c.pos = cropped_size;//c.preloaded_samples_size;

  {
    MutexAutolock l(m_ids);
    id2cache[id] = c;
  }
  // NOTE: Below this point we can no longer write to 'c'.

  // Only load next buffer if there are more data in the file to be loaded...
  if(c.pos < file->size) {
    cache_t& c = id2cache[id]; // Create new writeable 'c'.

    if(c.back == NULL) {
      c.back = new sample_t[CHUNKSIZE(framesize)];
    }

    cevent_t e =
      createLoadNextEvent(c.file, c.channel, c.pos, c.back);
    e.ready = &id2cache[id].ready;
    pushEvent(e);
  }

  return c.preloaded_samples; // return preloaded data
}

sample_t *CacheManager::next(cacheid_t id, size_t &size) 
{
  size = framesize;

  if(id == CACHE_DUMMYID) {
    assert(nodata);
    return nodata;
  }

  cache_t& c = id2cache[id];

  if(c.preloaded_samples) {

    // We are playing from memory:
    if(c.localpos < c.preloaded_samples_size) {
      sample_t *s = c.preloaded_samples + c.localpos;
      c.localpos += framesize;
      return s;
    }

    c.preloaded_samples = NULL; // Start using samples from disk.

  } else {

    // We are playing from cache:
    if(c.localpos < CHUNKSIZE(framesize)) {
      sample_t *s = c.front + c.localpos;
      c.localpos += framesize;
      return s;
    }
  }

  if(!c.ready) {
    printf("#%d: NOT READY!\n", id); // TODO: Count and show in UI?
    return nodata;
  }

  // Swap buffers
  std::swap(c.front, c.back);

  // Next time we go here we have already read the first frame.
  c.localpos = framesize;

  c.pos += CHUNKSIZE(framesize);
  
  if(c.pos < c.file->sf_info.frames) {
    if(c.back == NULL) {
      c.back = new sample_t[CHUNKSIZE(framesize)];
    }

    cevent_t e = createLoadNextEvent(c.file, c.channel, c.pos, c.back);
    c.ready = false;
    e.ready = &c.ready;
    pushEvent(e);
  } 

  if(!c.front) {
    printf("We shouldn't get here... ever!\n");
    assert(false);
    return nodata;
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

void CacheManager::setFrameSize(size_t framesize)
{
  if(framesize > this->framesize) {
    delete[] nodata;
    nodata = new sample_t[framesize];

    for(size_t i = 0; i < framesize; i++) {
      nodata[i] = 0;
    }
  }

  this->framesize = framesize;
}

void CacheManager::handleLoadNextEvent(cevent_t &e) 
{
  assert(files.find(e.file->filename) != files.end());
  readChunk(files[e.file->filename], e.channel, e.pos, CHUNKSIZE(framesize), e.buffer);
  *e.ready = true;
}

void CacheManager::handleCloseEvent(cevent_t &e) 
{
  cache_t& c = id2cache[e.id];

  auto f = files.find(c.file->filename);

  if(f != files.end()) {
    // Decrease ref count and close file if needed (in AFile destructor).
    files[c.file->filename]->ref--;
    if(files[c.file->filename]->ref == 0) {
      delete f->second;
      files.erase(f);
    }
  }

  delete[] c.front;
  delete[] c.back;

  {
    MutexAutolock l(m_ids);
    availableids.push_back(e.id);
  }
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

  // TODO: Check if event should be merged.
  {
    MutexAutolock l(m_events);
    eventqueue.push_back(e);
  }

  sem.post();
}

CacheManager::cevent_t
CacheManager::createLoadNextEvent(AFile *file, size_t channel, size_t pos,
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

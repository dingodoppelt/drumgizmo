/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            cachemanager.h
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
#ifndef __DRUMGIZMO_CACHEMANAGER_H__
#define __DRUMGIZMO_CACHEMANAGER_H__

#include <string>
#include <list>
#include <vector>

#include "thread.h"
#include "semaphore.h"
#include "mutex.h"

#include "audiotypes.h"
#include "audiofile.h"

#define CACHE_DUMMYID -2
#define CACHE_NOID -1

class AudioFile;
typedef int cacheid_t;


//TODO:
// 1: Move nodata initialisation to init method.
// 2: Make semaphore in thread to block init call until thread has been started.

//// next
// Pre: preloaded contains 2 x framesize. chunk size is framesize.
// allocate 2 chunks and copy initial_samples_needed to first buffer from
// preloaded data and enough to fill up the second buffer from preloaded
// returns the first buffer and its size in &size.
// get id from "free stack" and store pointers to buffers in id vector.
// event: open sndfile handle (if not already open) and increase refcount

//// next
// Return which ever buffer is the front, swap them and add event to load the
// next chunk.

//// close
// decrement file handle refcounter and close file if it is 0.
// free the 2 buffers
// (do not erase from the id vector), push index to
// "free stack" for reuse.

class CacheManager : public Thread {
public:
  /**
   * Empty constructor...
   */
  CacheManager();

  /**
   * Destroy object and stop thread if needed.
   */
  ~CacheManager();

  /**
   * Initialise cache manager and allocate needed resources
   * This method starts the cache manager thread.
   * This method blocks until the thread has been started.
   * @param poolsize The maximum number of parellel events supported. 
   */
  void init(size_t poolsize);

  /**
   * Stop thread and clean up resources.
   * This method blocks until the thread has stopped.
   */
  void deinit();

  /**
   * Register new cache entry.
   * Prepares an entry in the cache manager for future disk streaming.
   * @param file A pointer to the file which is to be streamed from.
   * @param initial_samples_needed The number of samples needed in the first
   *  read that is not nessecarily of framesize. This is the number of samples
   *  from the input event offset to the end of the frame in which it resides.
   *  initial_samples_needed <= framesize.
   * @param channel The channel to which the cache id will be bound.
   * @param [out] new_id The newly created cache id.
   * @return A pointer to the first buffer containing the
   *  'initial_samples_needed' number of samples.
   */
  sample_t *open(AudioFile *file, size_t initial_samples_needed, int channel,
                 cacheid_t &new_id);

  /**
   * Get next buffer.
   * Returns the next buffer for reading based on cache id.
   * This function will (if needed) schedule a new disk read to make sure that
   * data is available in the next call to this method.
   * @param id The cache id to read from.
   * @param [out] size The size of the returned buffer.
   * @return A pointer to the buffer.
   */
  sample_t *next(cacheid_t id, size_t &size);

  /**
   * Unregister cache entry.
   * Close associated file handles and free associated buffers.
   * @param id The cache id to close.
   */
  void close(cacheid_t id);

  ///! Internal thread main method - needs to be public.
  void thread_main();

private:

  void pushEvent();

  typedef struct {
    AudioFile *file;
    int channel;
    size_t pos;
    sample_t *front;
    sample_t *back;
  } cache_t;

  enum cmd_t {
    LOADNEXT = 0,
    CLEAN = 1
  };

  typedef struct {
    bool active;
    cacheid_t id;
    size_t pos;
    cmd_t cmd;
  } event_t;

  CacheManager::event_t createLoadNextEvent(cacheid_t id, size_t pos, cmd_t type);
  void loadNext(cacheid_t id);
  void pushEvent(event_t e);
  const cache_t getNextCache(cacheid_t id);

  // Protected by mutex
  std::list<event_t> eventqueue;
  std::list<cacheid_t> availableids; 
  std::vector<cache_t> id2cache; 
  
  Mutex m_events;
  Mutex m_ids;
  Mutex m_caches;

  Semaphore sem;

  int running;
};

#endif/*__DRUMGIZMO_CACHEMANAGER_H__*/

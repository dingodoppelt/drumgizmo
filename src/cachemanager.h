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

#define CACHEMANAGER_NOID -1
class AudioFile;
typedef int cacheid_t;

class CacheManager : public Thread {
public:
  CacheManager();
  ~CacheManager();

  void init(int poolsize);
  void deinit();

  void thread_main();

  // Pre: preloaded contains 2 x framesize. chunk size is framesize.
  // allocate 2 chunks and copy initial_samples_needed to first buffer from
  // preloaded data and enough to fill up the second buffer from preloaded
  // returns the first buffer and its size in &size.
  // get id from "free stack" and store pointers to buffers in id vector.
  // event: open sndfile handle (if not already open) and increase refcount
  sample_t *open(AudioFile *file, int initial_samples_needed, int channel, cacheid_t &new_id);

  // Return which ever buffer is the front, swap them and add event to load the
  // next chunk.
  sample_t *next(cacheid_t id, size_t &size);

  // decrement file handle refcounter and close file if it is 0.
  // free the 2 buffers
  // (do not erase from the id vector), push index to
  // "free stack" for reuse.
  void close(cacheid_t id);

  // id vector:
  // {
  //   AudioFile*
  //   channel
  //   buffer1, buffer2
  //   position
  //   (ready?)
  // }

private:

  void pushEvent();

  typedef struct {
    AudioFile *file;
    int channel;
    size_t pos;
    void *a;
    void *b;
  } cache_t;

  enum cmd_t {
    LOADNEXT = 0,
    CLEAN = 1
  };

  typedef struct {
    bool active;
    cacheid_t id;
    cmd_t cmd;
  } event_t;

  CacheManager::event_t createEvent(cacheid_t id, cmd_t type);
  void loadNext(cacheid_t id);
  void pushEvent(event_t e);

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

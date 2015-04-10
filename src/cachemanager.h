/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            cachemanager.h
 *
 *  Fri Apr 10 10:39:24 CEST 2015
 *  Copyright 2015 Bent Bisballe Nyeng
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
#ifndef __DRUMGIZMO_CACHEMANAGER_H__
#define __DRUMGIZMO_CACHEMANAGER_H__

#include <string>
#include <list>

class AudioFile;

class CacheManager {
public:

  class Request {
  public:
    std::string filename;
    std::map<unsigned int, void*> targets;
    size_t pos;
  };

  // Pre: preloaded contains 2 x framesize. chunk size is framesize.
  // allocate 2 chunks and copy initial_samples_needed to first buffer from
  // preloaded data and enough to fill up the second buffer from preloaded
  // returns the first buffer and its size in &size.
  // get id from "free stack" and store pointers to buffers in id vector.
  // event: open sndfile handle (if not already open) and increase refcount
  sample_t *hello(AudioFile *file, int initial_samples_needed, int channel, id_t &new_id);

  // Return which ever buffer is the front, swap them and add event to load the
  // next chunk.
  sample_t *getNextBuffer(id_t id, size_t &size);

  // decrement file handle refcounter and close file if it is 0.
  // free the 2 buffers
  // (do not erase from the id vector), push index to
  // "free stack" for reuse.
  void goodbye(id_t id);

  // id vector:
  // {
  //   AudioFile*
  //   channel
  //   buffer1, buffer2
  //   position
  //   (ready?)
  // }

private:
  class File {
  public:
    sndfile_t *handle;
    int refcounter;
  };

  std::list<Request> requests;
  std::map<AudioFile *, File>

};

#endif/*__DRUMGIZMO_CACHEMANAGER_H__*/

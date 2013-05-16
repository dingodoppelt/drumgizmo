/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumkitloader.cc
 *
 *  Thu Jan 17 20:54:14 CET 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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
#include "drumkitloader.h"

#include <hugin.hpp>

#include "drumkitparser.h"
#include "drumgizmo.h"

DrumKitLoader::DrumKitLoader(DrumGizmo *dg)
{
  drumgizmo = dg;
  is_done = false;
  quitit = false;
  skipit = false;
}

DrumKitLoader::~DrumKitLoader()
{
  if(!quitit) {
    stop();
  }
}

void DrumKitLoader::stop()
{
  quitit = true;
  semaphore.post();
  wait_stop();
}

void DrumKitLoader::skip()
{
  skipit = true;
  semaphore.post();
  skip_semaphore.wait();
}

bool DrumKitLoader::isDone()
{
  bool done;

  mutex.lock();
  done = is_done;
  mutex.unlock();

  return done;
}

void DrumKitLoader::loadKit(DrumKit *kit)
{
  this->kit = kit;

  mutex.lock();
  is_done = false;
  mutex.unlock();

  semaphore.post();
}

void DrumKitLoader::prepare(AudioFile* af)
{
  DEBUG(loader, "Preparing audiofile %p (%d in queue)\n",
        af, load_queue.size());
  mutex.lock();
  af->ref_count++;
  load_queue.push_back(af);
//  if(ref_count.find(af) == ref_count.end()) {
//    ref_count[af]++;
//  }
//  else {
//    ref_count[af] = 0;
//  }
  mutex.unlock(); 
  semaphore.post();
}

void DrumKitLoader::reset(AudioFile* af)
{
  mutex.lock();
  af->ref_count--;
  reset_queue.push_back(af);
  mutex.unlock();
  semaphore.post();
}

void DrumKitLoader::thread_main()
{
  while(1) {
    DEBUG(loader, "before sem\n");

    semaphore.wait();

    DEBUG(loader, "after sem\n");
    fflush(stdout);


    if(quitit) return;

    if(skipit) {
      skip_semaphore.post();
      skipit = false;
      continue;
    }

    if(!load_queue.empty()) {
      DEBUG(loader, "Loading remaining of audio file\n"); 
      AudioFile* af = load_queue.front();
      mutex.lock();
      load_queue.pop_front();
      mutex.unlock();
      af->loadNext();
    }
    else if(!reset_queue.empty()) {
      AudioFile* af = reset_queue.front();
      mutex.lock();
      if(af->ref_count <= 0) {
        af->reset();
        af->ref_count = 0;
      }
      reset_queue.pop_front();
      mutex.unlock();
    }
    else { // Initialize drum kit
      DEBUG(loader, "Initializing drum kit\n");
      unsigned int count = 0;

      if(kit && !kit->isValid()) goto finish;

      { // Count total number of files that need loading:
        Instruments::iterator i = kit->instruments.begin();
        while(i != kit->instruments.end()) {
          Instrument *instr = *i;
          if(instr && !instr->isValid()) goto finish;

          count += instr->audiofiles.size();
          i++;
        }
      }

      { // Now actually load them:
        unsigned int loaded = 0;
        Instruments::iterator i = kit->instruments.begin();
        while(i != kit->instruments.end()) {
          Instrument *instr = *i;
        
          if(instr && !instr->isValid()) goto finish;

          std::vector<AudioFile*>::iterator a = instr->audiofiles.begin();
          while(a != instr->audiofiles.end()) {
#if 0
#ifdef WIN32
            SleepEx(5000, FALSE);
#else
            usleep(5000);
#endif/*WIN32*/
#endif
            AudioFile *af = *a;

            if(af && !af->isValid()) goto finish;

            af->load();
          
            loaded++;

            LoadStatusMessage *ls = new LoadStatusMessage();
            ls->number_of_files = count;
            ls->numer_of_files_loaded = loaded;
            ls->current_file = af->filename;
            drumgizmo->sendGUIMessage(ls);
          
            a++;

            if(skipit) goto finish;
          }
        
          i++;
        }
      }

      mutex.lock();
      is_done = true;
      mutex.unlock();

    finish:
      continue;
    }
  }
}

#ifdef TEST_DRUMKITLOADER
//Additional dependency files
//deps:
//Required cflags (autoconf vars may be used)
//cflags:
//Required link options (autoconf vars may be used)
//libs:
#include "test.h"

TEST_BEGIN;

// TODO: Put some testcode here (see test.h for usable macros).

TEST_END;

#endif/*TEST_DRUMKITLOADER*/

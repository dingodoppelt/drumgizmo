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

DrumKitLoader::DrumKitLoader()
{
  is_done = false;
  quitit = false;
}

DrumKitLoader::~DrumKitLoader()
{
  quitit = true;
  semaphore.post();
  wait_stop();
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


void DrumKitLoader::thread_main()
{
  while(1) {
    DEBUG(loader, "before sem\n");
    semaphore.wait();
    DEBUG(loader, "after sem\n");

    if(quitit) return;

    Instruments::iterator i = kit->instruments.begin();
    while(i != kit->instruments.end()) {
      Instrument *instr = *i;

      std::vector<AudioFile*>::iterator a = instr->audiofiles.begin();
      while(a != instr->audiofiles.end()) {
        //        usleep(10000);
        AudioFile *af = *a;
        af->load();
        a++;
      }

      i++;
    }

    mutex.lock();
    is_done = true;
    mutex.unlock();
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
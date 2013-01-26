/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumkitloader.h
 *
 *  Thu Jan 17 20:54:13 CET 2013
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
#ifndef __DRUMGIZMO_DRUMKITLOADER_H__
#define __DRUMGIZMO_DRUMKITLOADER_H__

#include <string>

#include "thread.h"
#include "semaphore.h"
#include "mutex.h"

#include "drumkit.h"

class DrumKitLoader : public Thread {
public:
  DrumKitLoader();
  ~DrumKitLoader();

  void loadKit(DrumKit *kit);

  void thread_main();

  bool isDone();

private:
  Semaphore semaphore;
  DrumKit *kit;
  bool is_done;
  Mutex mutex;
  bool quitit;
};

#endif/*__DRUMGIZMO_DRUMKITLOADER_H__*/

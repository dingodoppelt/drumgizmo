/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            thread.h
 *
 *  Tue Jan 24 08:11:37 CET 2012
 *  Copyright 2012 Bent Bisballe Nyeng
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
#pragma once

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#endif/*WIN32*/

class Thread {
public:
  Thread();
  virtual ~Thread();

  void run();
  void wait_stop();

protected:
  virtual void thread_main() = 0;
  
private:
#ifdef WIN32
  HANDLE tid;
  static DWORD WINAPI
#else
  pthread_t tid;
  static void*
#endif/*WIN32*/
    thread_run(void *data);
};

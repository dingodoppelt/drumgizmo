/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            thread.cc
 *
 *  Sun Oct 31 12:12:20 CET 2004
 *  Copyright  2004 Bent Bisballe
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *    This file is part of MIaV.
 *
 *    MIaV is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    MIaV is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with MIaV; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "thread.h"

static void* thread_run(void *data) {
  Thread *t = (Thread*)data;
  t->thread_main();
  return NULL;
}

Thread::Thread()
{
}

Thread::~Thread()
{
}

void Thread::exec()
{
  pthread_attr_init(&attr);
  
  pthread_create(&tid, &attr, thread_run, this);
}

void Thread::wait_stop()
{
  pthread_join(tid, NULL);
}

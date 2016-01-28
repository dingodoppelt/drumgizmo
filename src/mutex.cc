/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            mutex.cc
 *
 *  Thu Nov 12 10:51:32 CET 2009
 *  Copyright 2009 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of Pracro.
 *
 *  Pracro is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Pracro is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Pracro; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "mutex.h"

#include <hugin.hpp>

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <errno.h>
#endif

struct mutex_private_t {
#ifdef WIN32
	HANDLE mutex;
#else
	pthread_mutex_t mutex;
#endif
};

Mutex::Mutex()
{
	prv = new struct mutex_private_t();
#ifdef WIN32
	prv->mutex = CreateMutex(nullptr,  // default security attributes
	                         FALSE, // initially not owned
	                         nullptr); // unnamed mutex
#else
	pthread_mutex_init (&prv->mutex, nullptr);
#endif
}

Mutex::~Mutex()
{
#ifdef WIN32
	CloseHandle(prv->mutex);
#else
	pthread_mutex_destroy(&prv->mutex);
#endif

	if(prv)
	{
	  delete prv;
	}
}

bool Mutex::try_lock()
{
#ifdef WIN32
	DEBUG(mutex, "%s\n", __PRETTY_FUNCTION__);
	DEBUG(mutex, "WAIT_OBJECT_0: %d, WaitForSingleObject: %d\n",
	      WAIT_OBJECT_0, WaitForSingleObject(prv->mutex, 0));

	return WaitForSingleObject(prv->mutex, 0) == WAIT_OBJECT_0;
#else
	return pthread_mutex_trylock(&prv->mutex) != EBUSY;
#endif
}

void Mutex::lock()
{
#ifdef WIN32
	WaitForSingleObject(prv->mutex, // handle to mutex
	                    INFINITE);  // no time-out interval
#else
	pthread_mutex_lock(&prv->mutex);
#endif
}

void Mutex::unlock()
{
#ifdef WIN32
	ReleaseMutex(prv->mutex);
#else
	pthread_mutex_unlock(&prv->mutex);
#endif
}

MutexAutolock::MutexAutolock(Mutex &m)
	: mutex(m)
{
	mutex.lock();
}

MutexAutolock::~MutexAutolock()
{
	mutex.unlock();
}

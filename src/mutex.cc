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
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "mutex.h"

#include <hugin.hpp>
#include "platform.h"

#if DG_PLATFORM == DG_PLATFORM_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#include <errno.h>
#endif

struct mutex_private_t {
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	HANDLE mutex;
#else
	pthread_mutex_t mutex;
#endif
};

Mutex::Mutex()
{
	prv = new struct mutex_private_t();
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	prv->mutex = CreateMutex(nullptr,  // default security attributes
	                         FALSE, // initially not owned
	                         nullptr); // unnamed mutex
#else
	pthread_mutex_init (&prv->mutex, nullptr);
#endif
}

Mutex::~Mutex()
{
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	CloseHandle(prv->mutex);
#else
	pthread_mutex_destroy(&prv->mutex);
#endif

	if(prv)
	{
	  delete prv;
	}
}

//! \return true if the function succeeds in locking the mutex for the thread.
//! false otherwise.
bool Mutex::try_lock()
{
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	DEBUG(mutex, "%s\n", __PRETTY_FUNCTION__);

	DWORD result = WaitForSingleObject(prv->mutex, 0);

	DEBUG(mutex, "WAIT_OBJECT_0: %lu, WAIT_TIMEOUT: %lu, result: %lu\n",
	      WAIT_OBJECT_0, WAIT_TIMEOUT, result);

	return result != WAIT_TIMEOUT;
#else
	return pthread_mutex_trylock(&prv->mutex) != EBUSY;
#endif
}

void Mutex::lock()
{
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	WaitForSingleObject(prv->mutex, // handle to mutex
	                    INFINITE);  // no time-out interval
#else
	pthread_mutex_lock(&prv->mutex);
#endif
}

void Mutex::unlock()
{
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
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

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            semaphore.cc
 *
 *  Sat Oct  8 17:44:13 CEST 2005
 *  Copyright  2005 Bent Bisballe Nyeng
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
#include "semaphore.h"

#include <hugin.hpp>
#include <limits>
#include <assert.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#else
// Make sure we don't include /this/ files header...
#include <../include/semaphore.h>
#include <errno.h>
#include <stdio.h>
#endif

struct semaphore_private_t {
#ifdef WIN32
	HANDLE semaphore;
#else
	sem_t semaphore;
#endif
};

Semaphore::Semaphore(std::size_t initial_count)
{
	prv = new struct semaphore_private_t();

#ifdef WIN32
	prv->semaphore = CreateSemaphore(nullptr,  // default security attributes
	                                 initial_count,
	                                 std::numeric_limits<LONG>::max(),
	                                 nullptr); // unnamed semaphore
#else
	const int pshared = 0;
	memset(&prv->semaphore, 0, sizeof(sem_t));
	sem_init(&prv->semaphore, pshared, initial_count);
#endif
}

Semaphore::~Semaphore()
{
#ifdef WIN32
	CloseHandle(prv->semaphore);
#else
	sem_destroy(&prv->semaphore);
#endif

	delete prv;
}

void Semaphore::post()
{
#ifdef WIN32
	ReleaseSemaphore(prv->semaphore, 1, NULL);
#else
	sem_post(&prv->semaphore);
#endif
}

bool Semaphore::wait(const std::chrono::milliseconds& timeout)
{
#ifdef WIN32
	DWORD ret = WaitForSingleObject(prv->semaphore, timeout.count());
	if(ret == WAIT_TIMEOUT)
	{
		return false;
	}

	assert(ret == WAIT_OBJECT_0);
#else
	struct timespec ts;

	// Get current time
	clock_gettime(CLOCK_REALTIME, &ts);

	// Add timeout
	time_t seconds = (time_t)(timeout.count() / 1000);
	ts.tv_sec += seconds;
	ts.tv_nsec += (long)((timeout.count() - (seconds * 1000)) * 1000000);

	// Make sure we don't overflow the nanoseconds.
	constexpr long nsec = 1000000000LL;
	if(ts.tv_nsec >= nsec)
	{
		ts.tv_nsec -= nsec;
		ts.tv_sec += 1;
	}

	int ret = sem_timedwait(&prv->semaphore, &ts);
	if(ret < 0)
	{
		if(errno == ETIMEDOUT)
		{
			return false;
		}

		perror("sem_timedwait()");
		assert(false);
	}
#endif

	return true;
}

void Semaphore::wait()
{
#ifdef WIN32
	WaitForSingleObject(prv->semaphore, INFINITE);
#else
	sem_wait(&prv->semaphore);
#endif
}

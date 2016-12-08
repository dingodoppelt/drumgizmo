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

#include "platform.h"

#if DG_PLATFORM != DG_PLATFORM_WINDOWS
// Make sure we don't include /this/ file's header...
#include <../include/semaphore.h>
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#endif

#if DG_PLATFORM == DG_PLATFORM_OSX
//#include <Multiprocessing.h>
#include <CoreServices/CoreServices.h>
#endif

#if DG_PLATFORM == DG_PLATFORM_FREEBSD
#include <sys/types.h>
#include <sys/lock.h>
#include <sys/sema.h>
#endif

struct semaphore_private_t {
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	HANDLE semaphore;
#elif DG_PLATFORM == DG_PLATFORM_OSX
	MPSemaphoreID semaphore;
#elif DG_PLATFORM == DG_PLATFORM_FREEBSD
	struct sema *semaphore;
#else
	sem_t semaphore;
#endif
};

Semaphore::Semaphore(std::size_t initial_count)
{
	prv = new struct semaphore_private_t();

#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	prv->semaphore = CreateSemaphore(nullptr,  // default security attributes
	                                 initial_count,
	                                 std::numeric_limits<LONG>::max(),
	                                 nullptr); // unnamed semaphore
#elif DG_PLATFORM == DG_PLATFORM_OSX
	MPCreateSemaphore(std::numeric_limits<std::uint32_t>::max(),
	                  initial_count,
	                  &prv->semaphore);
#elif DG_PLATFORM == DG_PLATFORM_FREEBSD
	sema_init(prv->semaphore, initial_count, "");
#else
	const int pshared = 0;
	memset(&prv->semaphore, 0, sizeof(sem_t));
	sem_init(&prv->semaphore, pshared, initial_count);
#endif
}

Semaphore::~Semaphore()
{
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	CloseHandle(prv->semaphore);
#elif DG_PLATFORM == DG_PLATFORM_OSX
	MPDeleteSemaphore(prv->semaphore);
#elif DG_PLATFORM == DG_PLATFORM_FREEBSD
#else
	sem_destroy(&prv->semaphore);
#endif

	delete prv;
}

void Semaphore::post()
{
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	ReleaseSemaphore(prv->semaphore, 1, nullptr);
#elif DG_PLATFORM == DG_PLATFORM_OSX
	MPSignalSemaphore(prv->semaphore);
#elif DG_PLATFORM == DG_PLATFORM_FREEBSD
	sema_destroy(prv->semaphore)
#else
	sem_post(&prv->semaphore);
#endif
}

bool Semaphore::wait(const std::chrono::milliseconds& timeout)
{
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	DWORD ret = WaitForSingleObject(prv->semaphore, timeout.count());
	if(ret == WAIT_TIMEOUT)
	{
		return false;
	}

	assert(ret == WAIT_OBJECT_0);
#elif DG_PLATFORM == DG_PLATFORM_OSX
	auto ret = MPWaitOnSemaphore(prv->semaphore,
	                             kDurationMillisecond * timeout.count());
	if(ret == kMPTimeoutErr)
	{
		return false;
	}
#elif DG_PLATFORM == DG_PLATFORM_FREEBSD
	sema_timedwait(prv->semaphore, timeout.count());
#else
	struct timespec ts;

	struct timeval now;
	int rv = gettimeofday(&now, nullptr);
	assert(rv == 0);

	ts.tv_sec  = now.tv_sec;
	ts.tv_nsec = now.tv_usec * 1000;

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
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	WaitForSingleObject(prv->semaphore, INFINITE);
#elif DG_PLATFORM == DG_PLATFORM_OSX
	MPWaitOnSemaphore(prv->semaphore, kDurationForever);
#elif DG_PLATFORM == DG_PLATFORM_FREEBSD
	sema_wait(prv->semaphore);
#else
	sem_wait(&prv->semaphore);
#endif
}

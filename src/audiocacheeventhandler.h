/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audiocacheeventhandler.h
 *
 *  Sun Jan  3 19:57:55 CET 2016
 *  Copyright 2016 Bent Bisballe Nyeng
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

#include <list>
#include <vector>
#include <mutex>

#include "thread.h"
#include "semaphore.h"

#include "audiocachefile.h"
#include "audiocacheidmanager.h"

class CacheEvent;

class AudioCacheEventHandler
	: protected Thread
{
public:
	AudioCacheEventHandler(AudioCacheIDManager& idManager);
	~AudioCacheEventHandler();

	//! Start event handler thread.
	//! This method blocks until the thread has actually been started.
	void start();

	//! Stop event handler thread.
	//! This method blocks until the thread has actually been stopped.
	void stop();

	//! Set thread status and start/stop thread accordingly.
	//! \param threaded Set to true to start thread or false to stop it.
	void setThreaded(bool threaded);

	//! Get current threaded status.
	bool getThreaded() const;

	//! Lock thread mutex.
	//! This methods are supplied to make this class lockable by std::lock_guard
	void lock();

	//! Unlock thread mutex.
	//! This methods are supplied to make this class lockable by std::lock_guard
	void unlock();

	void pushLoadNextEvent(AudioCacheFile* afile, size_t channel,
	                       size_t pos, sample_t* buffer,
	                       volatile bool* ready);
	void pushCloseEvent(cacheid_t id);

	void setChunkSize(size_t chunksize);
	size_t chunkSize();

	AudioCacheFile& openFile(const std::string& filename);

protected:
	AudioCacheFiles files;

	std::mutex mutex;

	void clearEvents();

	void handleLoadNextEvent(CacheEvent& e);
	void handleCloseEvent(CacheEvent& e);
	void handleCloseCache(cacheid_t cacheid);

	void handleEvent(CacheEvent& e);

	// From Thread
	void thread_main() override;

	void pushEvent(CacheEvent& e);

	std::list<CacheEvent>* eventqueue;

	bool threaded{false};
	Semaphore sem;
	Semaphore sem_run;
	bool running{false};

	AudioCacheIDManager& idManager;

	size_t chunksize{1024};
};
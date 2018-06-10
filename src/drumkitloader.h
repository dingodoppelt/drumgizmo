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
#pragma once

#include <string>
#include <list>
#include <mutex>
#include <vector>

#include "thread.h"
#include "semaphore.h"

#include "drumkit.h"
#include "settings.h"
#include "audioinputengine.h"
#include "audiocache.h"

class DrumkitDOM;
class InstrumentDOM;

//! This class is responsible for loading the drumkits in its own thread.
//! All interaction calls are simply modifying queues and not doing any
//! work in-sync with the caller.
//! This means that if loadKit(...) is called, one cannot assume that the
//! drumkit has actually been loaded when the call returns.
class DrumKitLoader
	: public Thread
{
public:
	DrumKitLoader(Settings& settings, DrumKit& kit, AudioInputEngine& ie,
	              Random& rand, AudioCache& audio_cache);

	~DrumKitLoader();

	//! Starts the loader thread.
	void init();

	//! Signal the loader thread to stop and waits for the threads to merge
	//! before returning.
	void deinit();

	bool loadkit(const std::string& file);

	//! Signal the loader to start loading all audio files contained in the kit.
	//! All other AudioFiles in queue will be removed before the new ones are
	//! scheduled.
	void loadKit(DrumKit *kit);

	//! Skip all queued AudioFiles.
	void skip();

	//! Set the framesize which will be used to preloading samples in next
	//! loadKit call.
	void setFrameSize(std::size_t framesize);

protected:
	void thread_main();

	Semaphore run_semaphore;
	Semaphore semaphore;
	Semaphore framesize_semaphore;
	std::mutex mutex;
	volatile bool running{false};
	std::list<AudioFile*> load_queue;
	std::size_t framesize{0};
	Settings& settings;
	SettingsGetter getter;
	DrumKit& kit;
	AudioInputEngine& ie;
	//MemChecker memchecker;
	Random& rand;
	AudioCache& audio_cache;
	std::size_t preload_samples{std::numeric_limits<std::size_t>::max()};
};

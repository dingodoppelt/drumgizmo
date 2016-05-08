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
#include "drumkitloader.h"

#include <iostream>

#include <hugin.hpp>

#include "drumkitparser.h"
#include "drumgizmo.h"
#include "audioinputenginemidi.h"

DrumKitLoader::DrumKitLoader(Settings& settings, DrumKit& kit,
                             AudioInputEngine& ie,
                             Resamplers& resamplers)
	: settings(settings)
	, getter(settings)
	, kit(kit)
	, ie(ie)
	, resamplers(resamplers)
{
	run();
	run_semaphore.wait(); // Wait for the thread to actually start.
}

DrumKitLoader::~DrumKitLoader()
{
	DEBUG(loader, "~DrumKitLoader() pre\n");

	if(running)
	{
		framesize_semaphore.post();
		stop();
	}

	DEBUG(loader, "~DrumKitLoader() post\n");
}

bool DrumKitLoader::loadkit(const std::string& file)
{
	settings.drumkit_load_status.store(LoadStatus::Idle);

	if(file == "")
	{
		settings.drumkit_load_status.store(LoadStatus::Error);
		return false;
	}

	DEBUG(drumgizmo, "loadkit(%s)\n", file.c_str());

	// Remove all queue AudioFiles from loader before we actually delete them.
	skip();

	// Delete all Channels, Instruments, Samples and AudioFiles.
	kit.clear();

	settings.drumkit_load_status.store(LoadStatus::Loading);

	DrumKitParser parser(settings, kit);
	if(parser.parseFile(file))
	{
		ERR(drumgizmo, "Drumkit parser failed: %s\n", file.c_str());
		settings.drumkit_load_status.store(LoadStatus::Error);
		return false;
	}

	// Check if there is enough free RAM to load the drumkit.
	if(!memchecker.enoughFreeMemory(kit))
	{
		printf("WARNING: "
		       "There doesn't seem to be enough RAM available to load the kit.\n"
		       "Trying to load it anyway...\n");
	}

	loadKit(&kit);

#ifdef WITH_RESAMPLER
	resamplers.setup(kit.getSamplerate(), settings.samplerate.load());
#endif/*WITH_RESAMPLER*/


	DEBUG(loadkit, "loadkit: Success\n");

	return true;
}

void DrumKitLoader::loadKit(DrumKit *kit)
{
//	std::lock_guard<std::mutex> guard(mutex);

	DEBUG(loader, "Create AudioFile queue from DrumKit\n");

	settings.number_of_files_loaded.store(0);

	// Count total number of files that need loading:
	settings.number_of_files.store(0);
	for(auto instr : kit->instruments)
	{
		settings.number_of_files.fetch_add(instr->audiofiles.size());
	}

	// Now actually queue them for loading:
	for(auto instr : kit->instruments)
	{
		for(auto audiofile : instr->audiofiles)
		{
			load_queue.push_back(audiofile);
		}
	}

	DEBUG(loader, "Queued %d (size: %d) AudioFiles for loading.\n",
	      (int)settings.number_of_files.load(), (int)load_queue.size());

	semaphore.post(); // Start loader loop.
}

void DrumKitLoader::stop()
{
	{
		std::lock_guard<std::mutex> guard(mutex);
		load_queue.clear();
	}

	running = false;
	semaphore.post();
	wait_stop();
}

void DrumKitLoader::skip()
{
	std::lock_guard<std::mutex> guard(mutex);
	load_queue.clear();
}

void DrumKitLoader::setFrameSize(size_t framesize)
{
	std::lock_guard<std::mutex> guard(mutex);
	this->framesize = framesize;
	framesize_semaphore.post(); // Signal that the framesize has been set.
}

void DrumKitLoader::thread_main()
{
	running = true;

	run_semaphore.post(); // Signal that the thread has been started.

	framesize_semaphore.wait(); // Wait until the framesize has been set.

	while(running)
	{
		size_t size;
		{
			std::lock_guard<std::mutex> guard(mutex);
			size = load_queue.size();
		}

		// Only sleep if queue is empty.
		if(size == 0)
		{
			semaphore.wait(std::chrono::milliseconds(1000));
		}

		bool newKit = false;
		if(getter.drumkit_file.hasChanged())
		{
			loadkit(getter.drumkit_file.getValue());
			newKit = true;
		}

		if(getter.midimap_file.hasChanged() || newKit)
		{
			auto ie_midi = dynamic_cast<AudioInputEngineMidi*>(&ie);
			std::cout << "ie_midi: " << (void*)ie_midi << std::endl;
			if(ie_midi)
			{
				settings.midimap_load_status.store(LoadStatus::Loading);
				bool ret = ie_midi->loadMidiMap(getter.midimap_file.getValue(),
				                                kit.instruments);
				if(ret)
				{
					settings.midimap_load_status.store(LoadStatus::Done);
				}
				else
				{
					settings.midimap_load_status.store(LoadStatus::Error);
				}
			}
		}

		std::string filename;
		{
			std::lock_guard<std::mutex> guard(mutex);

			if(load_queue.size() == 0)
			{
				continue;
			}

			AudioFile *audiofile = load_queue.front();
			load_queue.pop_front();
			filename = audiofile->filename;
			int preload_size = framesize * CHUNK_MULTIPLIER + framesize;
			if(preload_size < 1024)
			{
				preload_size = 1024;
			}

			// Note: Remove this line to enable diskstreaming
			//preload_size = ALL_SAMPLES;

			audiofile->load(preload_size);
		}

		settings.number_of_files_loaded.fetch_add(1);

		if(settings.number_of_files.load() == settings.number_of_files_loaded.load())
		{
			settings.drumkit_load_status.store(LoadStatus::Done);
		}
	}

	DEBUG(loader, "Loader thread finished.");
}

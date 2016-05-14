/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            dgreftest.cc
 *
 *  Thu May 12 17:50:08 CEST 2016
 *  Copyright 2016 Bent Bisballe Nyeng
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
#include <drumgizmo.h>

#include <string>
#include <iostream>
#include <cassert>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "compareoutputengine.h"
#include "midiinputengine.h"
#include "wavfileoutputengine.h"

#include <cpp11fix.h> // required for c++11

std::unique_ptr<AudioOutputEngine> createOutput(const std::string& name)
{
	struct stat sb;
	if((stat(name.c_str(), &sb) == -1) && (errno == ENOENT))
	{
		// Ref file doesn't exist - create it.
		return std::make_unique<WavfileOutputEngine>();
	}
	else
	{
		// Ref file exists, do compare.
		return std::make_unique<CompareOutputEngine>();
	}
}

int main(int argc, char* argv[])
{
	if(argc < 4)
	{
		std::cerr << "Usage: " << argv[0] << " testname drumkit midimap [seed]" << std::endl;
		return 1;
	}

	Settings settings;

	std::string test = argv[1];
	std::string kitfile = argv[2];
	std::string midimap = argv[3];

	unsigned int seed = 0;
	if(argc == 5)
	{
		seed = atoi(argv[4]);
	}

	std::string midifile = test + ".mid";
	std::string reffile = test + ".wav";

	MidifileInputEngine ie;
	ie.setParm("file", midifile.c_str());

	auto oe = createOutput(reffile);

	oe->setParm("file", reffile.c_str());
	oe->setParm("srate", "44100");

	DrumGizmo drumgizmo(settings, oe.get(), &ie);
	drumgizmo.setRandomSeed(seed);
	drumgizmo.setFreeWheel(true); // Run in-sync with disk-cache
	drumgizmo.setFrameSize(oe->getBufferSize());

//	settings.enable_resampling.store(false); // Bypass resampler - BROKEN

	settings.drumkit_file.store(kitfile);
	settings.midimap_file.store(midimap);

	printf("Loading drumkit, this may take a while:\n");

	while(settings.drumkit_load_status.load() != LoadStatus::Done)
	{
		usleep(10000);

		int total = settings.number_of_files.load();
		int loaded = settings.number_of_files_loaded.load();

		printf("\r%d of %d     ", loaded, total);
		fflush(stdout);

		if(settings.drumkit_load_status.load() == LoadStatus::Error)
		{
			printf("\nFailed to load \"%s\".\n", kitfile.c_str());
			return 1;
		}

		if(loaded == total)
		{
			//break;
		}
	}
	printf("\ndone\n");

	drumgizmo.setSamplerate(oe->getSamplerate());


	if(!drumgizmo.init())
	{
		printf("Failed init engine.\n");
		return 1;
	}

	drumgizmo.run(-1);

	return 0;
}

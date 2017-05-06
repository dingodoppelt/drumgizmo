/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumgizmoc.cc
 *
 *  Thu Sep 16 10:23:22 CEST 2010
 *  Copyright 2010 Bent Bisballe Nyeng
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
#include <iostream>

#include <config.h>
#include <getopt.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <chrono>
#include <thread>

#include <hugin.hpp>

#include "drumgizmo.h"
#include "drumgizmoc.h"
#include "enginefactory.h"
#include "bytesizeparser.h"

#include "event.h"

static const char version_str[] = "DrumGizmo v" VERSION "\n";

static const char copyright_str[] =
	"Copyright (C) 2008-2011 Bent Bisballe Nyeng - Aasimon.org.\n"
	"This is free software.  You may redistribute copies of it under the terms "
	"of\n"
	"the GNU Lesser General Public License <http://www.gnu.org/licenses/gpl.html>.\n"
	"There is NO WARRANTY, to the extent permitted by law.\n"
	"\n"
	"Written by Bent Bisballe Nyeng (deva@aasimon.org)\n";

static const char usage_str[] =
    "Usage: %s [options] drumkitfile\n"
    "Options:\n"
    "  -a, --async-load       Load drumkit in the background and start the "
    "engine immediately.\n"
    "  -i, --inputengine      dummy|test|jackmidi|midifile  Use said event "
    "input engine.\n"
    "  -I, --inputparms       parmlist  Set input engine parameters.\n"
    "  -o, --outputengine     dummy|alsa|jackaudio|wavfile  Use said audio "
    "output engine.\n"
    "  -O, --outputparms      parmlist  Set output engine parameters.\n"
    "  -e, --endpos           Number of samples to process, -1: infinite.\n"
#ifndef DISABLE_HUGIN
    "  -D, --debug ddd        Enable debug messages on 'ddd'; see hugin "
    "documentation for details\n"
#endif /*DISABLE_HUGIN*/
    "  -s, --streaming        Enable streaming.\n"
    "  -S, --streamingparms   Streaming options.\n"
    "  -v, --version          Print version information and exit.\n"
    "  -h, --help             Print this message and exit.\n"
    "\n"
    "Input engine parameters:\n"
    "  jackmidi:  midimap=<midimapfile>\n"
    "  midifile:  file=<midifile>, speed=<tempo> (default 1.0),\n"
    "             track=<miditrack> (default -1, all tracks)\n"
    "             midimap=<midimapfile>, loop=<true|false>\n"
    "  test:      p=<hit_propability> (default 0.1)\n"
    "             instr=<instrument> (default -1, random instrument)\n"
    "             len=<seconds> (default -1, forever)\n"
    "  dummy:\n"
    "\n"
    "Output engine parameters:\n"
    "  alsa:      dev=<device> (default 'default'), frames=<frames> (default "
    "32)\n"
    "             srate=<samplerate> (default 441000)\n"
    "  oss:       dev=<device> (default '/dev/dsp'), srate=<samplerate>,\n"
    "             max_fragments=<number> (default 4, see man page for more info),\n"
    "             fragment_size=<selector> (default 8, see man page for more info)\n"
    "  wavfile:   file=<filename> (default 'output'), srate=<samplerate> "
    "(default 44100)\n"
    "  jackaudio:\n"
    "  dummy:\n"
    "\n"
    "Streaming parameters:\n"
    "  limit:         Limit the amount of preloaded drumkit data to the size\n"
    // "  chunk_size:    chunk size in k,M,G\n"
    "\n";


int main(int argc, char* argv[])
{
	int c;
	Settings settings;
	settings.disk_cache_enable = false;
	std::string hugin_filter;
	unsigned int hugin_flags = 0;
#ifndef DISABLE_HUGIN
	hugin_flags = HUG_FLAG_DEFAULT;
#endif /*DISABLE_HUGIN*/

	std::string outputengine;
	std::string inputengine;
	std::string iparms;
	std::string oparms;
	std::string sparms;
	bool async = false;
	int endpos = -1;

	EngineFactory factory;

	int option_index = 0;
	static struct option long_options[] = {
		{"async-load", no_argument, 0, 'a'},
		{"inputengine", required_argument, 0, 'i'},
		{"inputparms", required_argument, 0, 'I'},
		{"outputengine", required_argument, 0, 'o'},
		{"outputparms", required_argument, 0, 'O'},
		{"endpos", required_argument, 0, 'e'},
#ifndef DISABLE_HUGIN
		{"debug", required_argument, 0, 'D'},
#endif /*DISABLE_HUGIN*/
		{"streaming", no_argument, 0, 's'},
		{"streamingparams", required_argument, 0, 'S'},
		{"version", no_argument, 0, 'v'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0}
	};
	while(1)
	{
		c = getopt_long(argc, argv, "hvpo:O:i:I:e:asS:"
#ifndef DISABLE_HUGIN
		                "D:"
#endif /*DISABLE_HUGIN*/
		                , long_options, &option_index);

		if(c == -1)
		{
			break;
		}

		switch(c)
		{
		case 'i':
			inputengine = optarg;
			if(inputengine == "help")
			{
				std::cout << "Available Input Engines = { ";
				for(auto const& name : factory.getInputEngines())
				{
					std::cout << name << " ";
				}
				std::cout << "}\n";
				return 0;
			}
			break;

#ifndef DISABLE_HUGIN
		case 'D':
			hugin_flags |= HUG_FLAG_USE_FILTER;
			hugin_filter = optarg;
			break;
#endif /*DISABLE_HUGIN*/

		case 'I':
			iparms = optarg;
			break;

		case 'o':
			outputengine = optarg;
			if(outputengine == "help")
			{
				std::cout << "Available Output Engines = { ";
				for(auto const& name : factory.getOutputEngines())
				{
					std::cout << name << " ";
				}
				std::cout << "}\n";
				return 0;
			}
			break;

		case 'O':
			oparms = optarg;
			break;

		case 'a':
			async = true;
			break;

		case 'e':
			endpos = atoi(optarg);
			break;

		case '?':
		case 'h':
			printf("%s", version_str);
			printf(usage_str, argv[0]);
			return 0;

		case 'v':
			printf("%s", version_str);
			printf("%s", copyright_str);
			return 0;

		case 's':
			settings.disk_cache_enable = true;
			break;

		case 'S':
			sparms = optarg;
			break;

		default:
			break;
		}
	}

	hug_status_t status = hug_init(hugin_flags, HUG_OPTION_FILTER,
	                               hugin_filter.c_str(), HUG_OPTION_END);
	if(status != HUG_STATUS_OK)
	{
		printf("Error: %d\n", status);
		return 1;
	}

	DEBUG(drumgizmo, "Debug enabled.");

	if(inputengine == "")
	{
		printf("Missing input engine\n");
		return 1;
	}

	auto ie = factory.createInput(inputengine);

	if(ie == NULL)
	{
		printf("Invalid input engine: %s\n", inputengine.c_str());
		return 1;
	}

	{
		std::string parm;
		std::string val;
		bool inval = false;
		for(size_t i = 0; i < iparms.size(); ++i)
		{
			if(iparms[i] == ',')
			{
				ie->setParm(parm, val);
				parm = "";
				val = "";
				inval = false;
				continue;
			}

			if(iparms[i] == '=')
			{
				inval = true;
				continue;
			}

			if(inval)
			{
				val += iparms[i];
			}
			else
			{
				parm += iparms[i];
			}
		}
		if(parm != "")
		{
			ie->setParm(parm, val);
		}
	}

	{
		if(sparms.size() != 0)
		{
			if(sparms[0] == ',' || sparms[sparms.size()-1] == ',')
			{
				std::cerr << "Invalid streamparms" << std::endl;
				return 1;
			}
			std::string token;
			std::istringstream tokenstream(sparms);
			while(getline(tokenstream, token, ','))
			{
				std::size_t index = token.find('=');
				if(index == std::string::npos || index == token.size()-1 || index == 0)
				{
					std::cerr << "Invalid streamparms" << std::endl;
					return 1;
				}

				std::string parm = token.substr(0, index);
				std::string value = token.substr(index+1);

				if(parm == "limit")
				{
					settings.disk_cache_upper_limit = byteSizeParser(value);
					if(settings.disk_cache_upper_limit == 0)
					{
						std::cerr << "Invalid argument for streamparms limit: " << value << std::endl;
						return 1;
					}
				}
				// else if(parm == "chunk_size")
				// {
				// 	settings.disk_cache_chunk_size = byteSizeParser(value);
				// 	if(settings.disk_cache_chunk_size == 0)
				// 	{
				// 		std::cerr << "Invalid argument for streamparms chunk_size: " << value << std::endl;
				// 		return 1;
				// 	}
				// }
				else
				{
					std::cerr << "Unknown streamingparms argument " << parm << std::endl;
					return 1;
				}
			}
		}
	}

	if(outputengine == "")
	{
		printf("Missing output engine\n");
		return 1;
	}

	auto oe = factory.createOutput(outputengine);

	if(oe == NULL)
	{
		printf("Invalid output engine: %s\n", outputengine.c_str());
		return 1;
	}

	{
		std::string parm;
		std::string val;
		bool inval = false;
		for(size_t i = 0; i < oparms.size(); ++i)
		{
			if(oparms[i] == ',')
			{
				oe->setParm(parm, val);
				parm = "";
				val = "";
				inval = false;
				continue;
			}

			if(oparms[i] == '=')
			{
				inval = true;
				continue;
			}

			if(inval)
			{
				val += oparms[i];
			}
			else
			{
				parm += oparms[i];
			}
		}
		if(parm != "")
		{
			oe->setParm(parm, val);
		}
	}

	std::string kitfile;

	if(option_index < argc)
	{
		while(optind < argc)
		{
			if(kitfile != "")
			{
				printf("Can only handle a single kitfile.\n");
				printf(usage_str, argv[0]);
				return 1;
			}
			kitfile = argv[optind++];
		}
		printf("\n");
	}
	else
	{
		printf("Missing kitfile.\n");
		printf(usage_str, argv[0]);
		return 1;
	}

	printf("Using kitfile: %s\n", kitfile.c_str());

	DrumGizmo gizmo(settings, *oe.get(), *ie.get());

	gizmo.setFrameSize(oe->getBufferSize());

	settings.drumkit_file.store(kitfile);

	printf("Loading drumkit, this may take a while:\n");

	if(!async)
	{
		while(settings.drumkit_load_status.load() != LoadStatus::Done)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			int total = settings.number_of_files.load();
			int loaded =  settings.number_of_files_loaded.load();

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
	}

	gizmo.setSamplerate(oe->getSamplerate());
	oe->onLatencyChange(gizmo.getLatency());

	if(!gizmo.init())
	{
		printf("Failed init engine.\n");
		return 1;
	}

	// former drumgizmo run call
	size_t pos = 0;
	size_t nsamples = oe->getBufferSize();
	sample_t *samples = (sample_t *)malloc(nsamples * sizeof(sample_t));

	gizmo.setFrameSize(oe->getBufferSize());

	ie->start();
	oe->start();

	while(gizmo.run(pos, samples, nsamples) == true)
	{
		pos += nsamples;
		if((endpos != -1) && (pos >= (size_t)endpos))
		{
			break;
		}
	}

	ie->stop();
	oe->stop();

	free(samples);
	// end former drumgizmo run call

	printf("Quit.\n");
	fflush(stdout);

	hug_close();

	return 0;
}

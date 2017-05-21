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
#include <getoptpp.hpp>

#include <string.h>
#include <sstream>
#include <chrono>
#include <thread>
#include <wordexp.h>

#include <hugin.hpp>

#include "drumgizmo.h"
#include "drumgizmoc.h"
#include "enginefactory.h"
#include "bytesizeparser.h"

#include "event.h"


typedef struct parm_token
{
	std::string key;
	std::string value;
} parm_token_t;


static std::string version()
{
	std::ostringstream output;
	output << "DrumGizmo v" << VERSION << std::endl;
	return output.str();
}


static std::string copyright()
{
	std::ostringstream output;
	output << "Copyright (C) 2008-2011 Bent Bisballe Nyeng - Aasimon.org.\n";
	output << "This is free software.  You may redistribute copies of it under the terms ";
	output << "of\n";
	output << "the GNU Lesser General Public License <http://www.gnu.org/licenses/gpl.html>.\n";
	output << "There is NO WARRANTY, to the extent permitted by law.\n";
	output << "\n";
	output << "Written by Bent Bisballe Nyeng (deva@aasimon.org)\n";
	return output.str();
}


static std::string usage(std::string name)
{
	std::ostringstream output;
	output << "Usage: " << name << " [options] drumkitfile\n";
	output << "Options:\n";
	output << "  -a, --async-load       Load drumkit in the background and start the ";
	output << "engine immediately.\n";
	output << "  -i, --inputengine      dummy|test|jackmidi|midifile  Use said event ";
	output << "input engine.\n";
	output << "  -I, --inputparms       parmlist  Set input engine parameters.\n";
	output << "  -o, --outputengine     dummy|alsa|jackaudio|wavfile  Use said audio ";
	output << "output engine.\n";
	output << "  -O, --outputparms      parmlist  Set output engine parameters.\n";
	output << "  -e, --endpos           Number of samples to process, -1: infinite.\n";
#ifndef DISABLE_HUGIN
	output << "  -D, --debug ddd        Enable debug messages on 'ddd'; see hugin ";
	output << "documentation for details\n";
#endif /*DISABLE_HUGIN*/
	output << "  -r, --no-resampling    Disable resampling.\n";
	output << "  -s, --streaming        Enable streaming.\n";
	output << "  -S, --streamingparms   Streaming options.\n";
	output << "  -v, --version          Print version information and exit.\n";
	output << "  -h, --help             Print this message and exit.\n";
	output << "\n";
	output << "Input engine parameters:\n";
	output << "  jackmidi:  midimap=<midimapfile>\n";
	output << "  midifile:  file=<midifile>, speed=<tempo> (default 1.0),\n";
	output << "             track=<miditrack> (default -1, all tracks)\n";
	output << "             midimap=<midimapfile>, loop=<true|false>\n";
	output << "  ossmidi:   midimap=<midimapfile>, dev=<device> (default '/dev/dsp')\n";
	output << "  test:      p=<hit_propability> (default 0.1)\n";
	output << "             instr=<instrument> (default -1, random instrument)\n";
	output << "             len=<seconds> (default -1, forever)\n";
	output << "  dummy:\n";
	output << "\n";
	output << "Output engine parameters:\n";
	output << "  alsa:      dev=<device> (default 'default'), frames=<frames> (default ";
	output << "32)\n";
	output << "             srate=<samplerate> (default 441000)\n";
	output << "  oss:       dev=<device> (default '/dev/dsp'), srate=<samplerate>,\n";
	output << "             max_fragments=<number> (default 4, see man page for more info),\n";
	output << "             fragment_size=<selector> (default 8, see man page for more info)\n";
	output << "  wavfile:   file=<filename> (default 'output'), srate=<samplerate> ";
	output << "(default 44100)\n";
	output << "  jackaudio:\n";
	output << "  dummy:\n";
	output << "\n";
	output << "Streaming parameters:\n";
	output << "  limit:         Limit the amount of preloaded drumkit data to the size\n";
	// output << "  chunk_size:    chunk size in k,M,G\n"
	output << "\n";
	return output.str();
}


std::vector<parm_token_t> parse_parameters(std::string &parms)
{
	std::vector<parm_token_t> result;
	std::string parm;
	std::string val;
	bool inval = false;
	wordexp_t exp_result;
	for(size_t i = 0; i < parms.size(); ++i)
	{
		if(parms[i] == ',')
		{
			int error = wordexp(val.data(), &exp_result, 0);
			if(error)
			{
				std::cerr << "Wrong argument: ";
				std::cerr << parm << " = " << val << std::endl;
				exit(1);
			}
			result.push_back({parm, exp_result.we_wordv[0]});
			parm = "";
			val = "";
			inval = false;
			continue;
		}

		if(parms[i] == '=')
		{
			inval = true;
			continue;
		}

		if(inval)
		{
			val += parms[i];
		}
		else
		{
			parm += parms[i];
		}
	}
	if(parm != "")
	{
		int error = wordexp(val.data(), &exp_result, 0);
		if(error)
		{
			std::cerr << "Wrong argument: ";
			std::cerr << parm << " = " << val << std::endl;
			exit(1);
		}
		result.push_back({parm, exp_result.we_wordv[0]});
	}
	return result;
}


int main(int argc, char* argv[])
{
	EngineFactory factory;
	Settings settings;
	bool async = false;
	dg::Options opt;
	int endpos = -1;
	std::string kitfile;
	std::unique_ptr<AudioInputEngine> ie;
	std::unique_ptr<AudioOutputEngine> oe;
	std::string hugin_filter;
	unsigned int hugin_flags = 0;
#ifndef DISABLE_HUGIN
	hugin_flags = HUG_FLAG_DEFAULT;
#endif /*DISABLE_HUGIN*/

	opt.add("async-load", no_argument, 'a', [&]() {
		async = true;
	});

	opt.add("inputengine", required_argument, 'i', [&]() {
		std::string engine = optarg;
		if(engine == "help")
		{
			std::cout << "Available Input Engines = { ";
			for(auto const& name : factory.getInputEngines())
			{
				std::cout << name << " ";
			}
			std::cout << "}\n";
			exit(0);
		}
		if(engine == "")
		{
			std::cerr << "Missing input engine" << std::endl;;
			exit(1);
		}
		ie = factory.createInput(engine);
		if(ie == NULL)
		{
			std::cerr << "Invalid input engine: " << engine << std::endl;
			exit(1);
		}
	});

	opt.add("inputparms", required_argument, 'I', [&]() {
		std::string parms = optarg;
		auto tokens = parse_parameters(parms);
		for(auto &token : tokens)
		{
			ie->setParm(token.key, token.value);
		}
	});

	opt.add("outputengine", required_argument, 'o', [&]() {
		std::string engine = optarg;
		if(engine == "help")
		{
			std::cout << "Available Output Engines = { ";
			for(auto const& name : factory.getOutputEngines())
			{
				std::cout << name << " ";
			}
			std::cout << " }\n";
			exit(0);
		}
		if(engine == "")
		{
			std::cerr << "Missing output engine" << std::endl;
			exit(1);
		}
		oe = factory.createOutput(engine);
		if(ie == NULL)
		{
			std::cerr << "Invalid output engine: " << engine << std::endl;;
			exit(1);
		}
	});

	opt.add("outputparms", required_argument, 'O', [&]() {
		std::string parms = optarg;
		auto tokens = parse_parameters(parms);
		for(auto &token : tokens)
		{
			oe->setParm(token.key, token.value);
		}
	});

	opt.add("endpos", required_argument, 'e', [&]() {
			try
			{
				endpos = std::stoi(optarg);
			}
			catch(...)
			{
				std::cerr << "Invalid endpos size " << optarg << std::endl;
			}
	});

#ifndef DISABLE_HUGIN
	opt.add("debug", required_argument, 'D', [&]() {
		hugin_flags |= HUG_FLAG_USE_FILTER;
		hugin_filter = optarg;
	});
#endif /*DISABLE_HUGIN*/

	opt.add("no-resampling", no_argument, 'r', [&]() {
		settings.enable_resampling = false;
	});

	opt.add("streaming", no_argument, 's', [&]() {
		settings.disk_cache_enable = true;
	});

	opt.add("streamingparms", required_argument, 'S', [&]() {
		std::string parms = optarg;
		auto tokens = parse_parameters(parms);
		for(auto &token : tokens)
		{
			if(token.key == "limit")
			{
				settings.disk_cache_upper_limit = byteSizeParser(token.value);
				if(settings.disk_cache_upper_limit == 0)
				{
					std::cerr << "Invalid argument for streamparms limit: " << token.value << std::endl;
					exit(1);
				}
			}
			// else if(token.key == "chunk_size")
			// {
			// 	settings.disk_cache_chunk_size = byteSizeParser(token.value);
			// 	if(settings.disk_cache_chunk_size == 0)
			// 	{
			// 		std::cerr << "Invalid argument for streamparms chunk_size: " << token.value << std::endl;
			// 		exit(1);
			// 	}
			// }
			else
			{
				std::cerr << "Unknown streamingparms argument " << token.key << std::endl;
				exit(1);
			}
		}
	});

	opt.add("version", no_argument, 'v', [&]() {
			std::cout << version();
			std::cout << copyright();
			exit(0);
		});

	opt.add("help", no_argument, 'h', [&]() {
			std::cout << version();
			std::cout << usage(argv[0]);
			exit(0);
		});

	if(!opt.process(argc, argv))
	{
		return 1;
	}
	hug_status_t status = hug_init(hugin_flags, HUG_OPTION_FILTER,
	                               hugin_filter.data(), HUG_OPTION_END);
	if(status != HUG_STATUS_OK)
	{
		std::cerr << "Error: " << status << std::endl;
		return 1;
	}
	DEBUG(drumgizmo, "Debug enabled.");

	if(opt.arguments().empty())
	{
		std::cerr << "Missing kitfile." << std::endl;
		std::cerr << usage(argv[0]) << std::endl;
		return 1;
	}
	else if(opt.arguments().size() > 1)
	{
		std::cerr << "Can only handle a single kitfile." << std::endl;
		std::cerr << usage(argv[0]) << std::endl;
		return 1;
	}
	else
	{
		kitfile = opt.arguments()[0];
		std::ifstream tmp_kit(kitfile);
		if(!tmp_kit.good())
		{
			std::cerr << "Can not open " << kitfile << std::endl;
			return 1;
		}
	}

	if(ie == nullptr)
	{
		std::cerr << "No input engine selected.\n";
		std::cout << usage(argv[0]);
		return 1;
	}

	if(oe == nullptr)
	{
		std::cerr << "No output engine selected.\n";
		std::cout << usage(argv[0]);
		return 1;
	}

	std::cout << "Using kitfile: " << kitfile << std::endl;

	DrumGizmo gizmo(settings, *oe.get(), *ie.get());

	gizmo.setFrameSize(oe->getBufferSize());

	settings.drumkit_file.store(kitfile);
	settings.reload_counter++;

	std::cout << "Loading drumkit, this may take a while:" << std::endl;

	if(!async)
	{
		while(settings.drumkit_load_status.load() != LoadStatus::Done)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			int total = settings.number_of_files.load();
			int loaded =  settings.number_of_files_loaded.load();

			std::cout << "\r" << loaded << " of " << total << std::flush;

			if(settings.drumkit_load_status.load() == LoadStatus::Error)
			{
				std::cout << "\nFailed to load " << kitfile << std::endl;
				return 1;
			}

			if(loaded == total)
			{
				//break;
			}
		}
		std::cout << "\ndone" << std::endl;
	}

	gizmo.setSamplerate(oe->getSamplerate());
	oe->onLatencyChange(gizmo.getLatency());

	if(!gizmo.init())
	{
		std::cout << "Failed init engine." << std::endl;
		return 1;
	}

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
	std::cout << "Quit." << std::endl;
	hug_close();

	return 0;
}

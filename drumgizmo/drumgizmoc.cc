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
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#ifdef HAVE_WORDEXP
#include <wordexp.h>
#else
#include <glob.h>
#endif

#include <hugin.hpp>

#include "drumgizmo.h"
#include "enginefactory.h"
#include "bytesizeparser.h"

#include "event.h"

#include "nolocale.h"

struct ParmToken
{
	std::string key;
	std::string value;
};

static std::string version()
{
	std::ostringstream output;
	output << "DrumGizmo v" << VERSION << std::endl;
	return output.str();
}

static std::string copyright()
{
	std::ostringstream output;
	output << "Copyright (C) 2008-2020 DrumGizmo team - DrumGizmo.org.\n";
	output << "This is free software.  You may redistribute copies of it under the terms ";
	output << "of\n";
	output << "the GNU Lesser General Public License <http://www.gnu.org/licenses/gpl.html>.\n";
	output << "There is NO WARRANTY, to the extent permitted by law.\n";
	output << "\n";
	return output.str();
}

static std::string usage(const std::string& name)
{
	std::ostringstream output;
	output <<
		"==============================================================================\n"
		"This is the command line version of DrumGizmo. The graphical user interface is\n"
		"only available in the plugins.\n"
		"==============================================================================\n"
		"\n"
		"Usage: " << name << " [options] drumkitfile\n";
	return output.str();
}

static std::string arguments()
{
	std::ostringstream output;
	output <<
		"Input engine parameters:\n"
		"  jackmidi:  midimap=<midimapfile>\n"
		"  alsamidi:  midimap=<midimapfile>\n"
		"  midifile:  file=<midifile>, speed=<tempo> (default 1.0),\n"
		"             track=<miditrack> (default -1, all tracks)\n"
		"             midimap=<midimapfile>, loop=<true|false>\n"
		"  ossmidi:   midimap=<midimapfile>, dev=<device> (default '/dev/midi')\n"
		"  test:      p=<hit_propability> (default 0.1)\n"
		"             instr=<instrument> (default -1, random instrument)\n"
		"             len=<seconds> (default -1, forever)\n"
		"  dummy:\n"
		"\n"
		"Output engine parameters:\n"
		"  alsa:      dev=<device> (default 'default'), frames=<frames> (default "
		"32),\n"
		"             periods=<periods> (default 3)\n"
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
		//"  chunk_size:    chunk size in k,M,G\n"
		"\n"
		"Timing humanizer parameters:\n"
		"  laidback:      Move notes ahead or behind in time in ms [+/-100].\n"
		"  tightness:     Control the tightness of the drummer. [0,1]\n"
		"  regain:        Control how fast the drummer catches up the timing. [0,1]\n"
		"\n"
		"Velocity humanizer parameters:\n"
		"  attack:        How quickly the velocity gets reduced when playing fast notes.\n"
		"                 Lower values result in faster velocity reduction.\n"
		"  release:       How quickly the drummer regains the velocity\n"
		"                 when there are spaces between the notes.\n"
		"                 Lower values result in faster regain. [0,1]\n"
		"  stddev:        The standard-deviation of the velocity humanization.\n"
		"                 Higher value makes it more likely that a sample further\n"
		"                 away from the input velocity will be played. [0,4.5]\n"
		"\n"
		"Sample selection parameters:\n"
		"  close:         The importance given to choosing a sample close to\n"
		"                 the actual velocity value (after humanization). [0,1]\n"
		"  diverse:       The importance given to choosing samples\n"
		"                 which haven't been played recently. [0,1]\n"
		"  random:        The amount of randomness added. [0,1]\n"
		"\n";
	return output.str();
}

std::vector<ParmToken> parseParameters(std::string &parms)
{
	std::vector<ParmToken> result;
	std::string parm;
	std::string val;
	bool inval = false;
#ifdef HAVE_WORDEXP
	wordexp_t exp_result;
#else
	glob_t g;
#endif
	for(size_t i = 0; i < parms.size(); ++i)
	{
		if(parms[i] == ',')
		{
		#ifdef HAVE_WORDEXP
			int error = wordexp(val.data(), &exp_result, 0);
		#else
			int error = glob(val.data(), 0, NULL, &g);
		#endif
			if(error)
			{
				std::cerr << "Wrong argument: ";
				std::cerr << parm << " = " << val << std::endl;
				exit(1);
			}
		#ifdef HAVE_WORDEXP
			result.push_back({parm, exp_result.we_wordv[0]});
		#else
			result.push_back({parm, g.gl_pathv[0]});
		#endif
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
	#ifdef HAVE_WORDEXP
		int error = wordexp(val.data(), &exp_result, 0);
	#else
		int error = glob(val.data(), 0, NULL, &g);
	#endif
		if(error)
		{
			std::cerr << "Wrong argument: ";
			std::cerr << parm << " = " << val << std::endl;
			exit(1);
		}
	#ifdef HAVE_WORDEXP
		result.push_back({parm, exp_result.we_wordv[0]});
	#else
		result.push_back({parm, g.gl_pathv[0]});
	#endif
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

	// if no arguments are passed, just print usage and exit
	if (argc == 1)
	{
		std::cout << version();
		std::cout << usage(argv[0]);
		return 0;
	}

	opt.add("async-load", no_argument, 'a',
	        "Load drumkit in the background and start the engine immediately.",
	        [&]()
	        {
		        async = true;
		        return 0;
	        });

	opt.add("bleed", required_argument, 'b',
	        "Set and enable master bleed.",
	        [&]()
	        {
		        float bleed = atof_nol(optarg);
		        settings.enable_bleed_control = true;
		        settings.master_bleed = bleed;
		        return 0;
	        });

	opt.add("inputengine", required_argument, 'i',
	        "dummy|test|jackmidi|alsamidi|midifile  Use said event input engine.",
	        [&]()
	        {
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
			        std::cerr << "Missing input engine" << std::endl;
			        return 1;
		        }
		        ie = factory.createInput(engine);
		        if(ie == NULL)
		        {
			        std::cerr << "Invalid input engine: " << engine << std::endl;
			        return 1;
		        }
		        return 0;
	        });

	opt.add("inputparms", required_argument, 'I',
	        "Set input engine parameters.",
	        [&]()
	        {
		        std::string parms = optarg;
		        auto tokens = parseParameters(parms);
		        for(auto& token : tokens)
		        {
			        ie->setParm(token.key, token.value);
		        }
		        return 0;
	        });

	opt.add("outputengine", required_argument, 'o',
	        "dummy|alsa|jackaudio|wavfile  Use said audio output engine.",
	        [&]()
	        {
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
			        return 1;
		        }
		        oe = factory.createOutput(engine);
		        if(ie == NULL)
		        {
			        std::cerr << "Invalid output engine: " << engine << std::endl;
			        return 1;
		        }
		        return 0;
	        });

	opt.add("outputparms", required_argument, 'O',
	        "Set output engine parameters.",
	        [&]()
	        {
		        std::string parms = optarg;
		        auto tokens = parseParameters(parms);
		        for(auto& token : tokens)
		        {
			        oe->setParm(token.key, token.value);
		        }
		        return 0;
	        });

	opt.add("endpos", required_argument, 'e',
	        "Number of samples to process, -1: infinite.",
	        [&]()
	        {
		        try
		        {
			        endpos = std::stoi(optarg);
		        }
		        catch(...)
		        {
			        std::cerr << "Invalid endpos size " << optarg << std::endl;
		        }
		        return 0;
	        });

#ifndef DISABLE_HUGIN
	opt.add("debug", required_argument, 'D',
	        "Enable debug messages on 'ddd' see hugin documentation for details.",
	        [&]()
	        {
		        hugin_flags |= HUG_FLAG_USE_FILTER;
		        hugin_filter = optarg;
		        return 0;
	        });
#endif /*DISABLE_HUGIN*/

	opt.add("no-resampling", no_argument, 'r',
	        "Disable resampling.",
	        [&]()
	        {
		        settings.enable_resampling = false;
		        return 0;
	        });

	opt.add("streaming", no_argument, 's',
	        "Enable streaming.",
	        [&]()
	        {
		        settings.disk_cache_enable = true;
		        return 0;
	        });

	opt.add("streamingparms", required_argument, 'S',
	        "Streaming options.",
	        [&]()
	        {
		        std::string parms = optarg;
		        auto tokens = parseParameters(parms);
		        for(auto& token : tokens)
		        {
			        if(token.key == "limit")
			        {
				        settings.disk_cache_upper_limit = byteSizeParser(token.value);
				        if(settings.disk_cache_upper_limit == 0)
				        {
					        std::cerr << "Invalid argument for streamparms limit: " <<
						        token.value << std::endl;
					        return 1;
				        }
			        }
			        //else if(token.key == "chunk_size")
			        //{
			        //	settings.disk_cache_chunk_size = byteSizeParser(token.value);
			        //	if(settings.disk_cache_chunk_size == 0)
			        //	{
			        //		std::cerr << "Invalid argument for streamparms chunk_size: " <<
			        //			token.value << std::endl;
			        //		return 1;
			        //	}
			        // }
			        else
			        {
				        std::cerr << "Unknown streamingparms argument " << token.key <<
					        std::endl;
				        return 1;
			        }
		        }
		        return 0;
	        });

	// Default is to disable timing humanizer
	settings.enable_latency_modifier.store(false);

	opt.add("timing-humanizer", no_argument, 't',
	        "Enable moving of notes in time.\nNOTE: Adds latency to the output so do not use this with a real-time drumkit.",
	        [&]()
	        {
		        settings.enable_latency_modifier.store(true);
		        return 0;
	        });

	opt.add("timing-humanizerparms", required_argument, 'T',
	        "Timing humanizer options.",
	        [&]()
	        {
		        std::string parms = optarg;
		        auto tokens = parseParameters(parms);
		        for(auto& token : tokens)
		        {
			        if(token.key == "laidback")
			        {
				        // Input range [+/- 100] ms
				        auto val = atof_nol(token.value.data());
				        if(val < -100.0 || val > 100.0)
				        {
					        std::cerr << "laidback range is +/- 100 and is in ms.\n";
					        return 1;
				        }
				        settings.latency_laid_back_ms.store(val);
			        }
			        else if(token.key == "tightness")
			        {
				        // Input range [0, 1]
				        auto val = atof_nol(token.value.data());
				        if(val < 0.0 || val > 1.0)
				        {
					        std::cerr << "tightness range is [0, 1].\n";
					        return 1;
				        }
				        settings.latency_stddev.store((-1.0 * val + 1.0) * 20.0);
			        }
			        else if(token.key == "regain")
			        {
				        auto val = atof_nol(token.value.data());
				        if(val < 0.0 || val > 1.0)
				        {
					        std::cerr << "regain range is [0, 1].\n";
					        return 1;
				        }
				        settings.latency_regain.store(val);
			        }
			        else
			        {
				        std::cerr << "Unknown timing-humanizerparms argument " << token.key << std::endl;
				        return 1;
			        }
		        }
		        return 0;
	        });

	// Default is to disable the velocity humanizer
	settings.enable_velocity_modifier.store(false);

	opt.add("velocity-humanizer", no_argument, 'x',
	        "Enables adapting input velocities to make it sound more realistic.",
	        [&]()
	        {
		        settings.enable_velocity_modifier.store(true);
		        return 0;
	        });

	opt.add("velocity-humanizerparms", required_argument, 'X',
	        "Velocity humanizer options.",
	        [&]()
	        {
		        std::string parms = optarg;
		        auto tokens = parseParameters(parms);
		        for(auto& token : tokens)
		        {
			        if (token.key == "attack")
			        {
				        auto val = atof_nol(token.value.data());
				        if(val < 0 || val > 1.0)
				        {
					        std::cerr << "attack range is [0, 1].\n";
					        return 1;
				        }
				        settings.velocity_modifier_weight.store(val);
			        }
			        else if (token.key == "release")
			        {
				        auto val = atof_nol(token.value.data());
				        if(val < 0 || val > 1.0)
				        {
					        std::cerr << "release range is [0, 1].\n";
					        return 1;
				        }
				        settings.velocity_modifier_falloff.store(val);
			        }
			        else if(token.key == "stddev")
			        {
				        auto val = atof_nol(token.value.data());
				        if(val < 0 || val > 4.5)
				        {
					        std::cerr << "stddev range is [0, 4.5].\n";
					        return 1;
				        }
				        settings.velocity_stddev.store(val);
			        }
			        else
			        {
				        std::cerr << "Unknown velocity-humanizerparms argument " << token.key << std::endl;
				        return 1;
			        }
		        }
		        return 0;
	        });

	opt.add("parameters", required_argument, 'p',
	        "Parameters for sample selection algorithm.",
	        [&]()
	        {
		        std::string parms = optarg;
		        auto tokens = parseParameters(parms);
		        for(auto& token : tokens)
		        {
			        if(token.key == "close")
			        {
				        auto val = atof_nol(token.value.data());
				        if(val < 0 || val > 1)
				        {
					        std::cerr << "close range is [0, 1].\n";
					        return 1;
				        }
				        settings.sample_selection_f_close.store(val);
			        }
			        else if(token.key == "diverse")
			        {
				        auto val = atof_nol(token.value.data());
				        if(val < 0 || val > 1)
				        {
					        std::cerr << "diverse range is [0, 1].\n";
					        return 1;
				        }
				        settings.sample_selection_f_diverse.store(val);
			        }
			        else if(token.key == "random")
			        {
				        auto val = atof_nol(token.value.data());
				        if(val < 0 || val > 1)
				        {
					        std::cerr << "random range is [0, 1].\n";
					        return 1;
				        }
				        settings.sample_selection_f_random.store(val);
			        }
			        else
			        {
				        std::cerr << "Unknown parameters argument " << token.key << std::endl;
				        return 1;
			        }
		        }
		        return 0;
	        });

	opt.add("version", no_argument, 'v',
	        "Print version information and exit.",
	        [&]()
	        {
		        std::cout << version();
		        std::cout << copyright();
		        exit(0);
		        return 0;
	        });

	opt.add("help", no_argument, 'h',
	        "Print this message and exit.",
	        [&]()
	        {
		        std::cout << version();
		        std::cout << usage(argv[0]);
		        std::cout << "Options:\n";
		        opt.help();
		        std::cout << "\nParameters:\n";
		        std::cout << arguments();
		        exit(0);
		        return 0;
	        });

	if(opt.process(argc, argv) != 0)
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

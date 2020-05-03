/* -*- Mode: c++ -*- */
/***************************************************************************
 *            dgvalidator.cc
 *
 *  Sun Jan 27 10:44:44 CET 2019
 *  Copyright 2019 Bent Bisballe Nyeng
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
#include <dgxmlparser.h>
#include <path.h>
#include <domloader.h>
#include <random.h>
#include <settings.h>
#include <drumkit.h>
#include <iostream>
#include <string>
#include <hugin.hpp>
#include <getoptpp.hpp>
#include <sstream>

#include <config.h>

namespace
{

int verbosity{1};

void logger(LogLevel level, const std::string& message)
{
	switch(level)
	{
	case LogLevel::Info:
		if(verbosity < 3)
		{
			return;
		}
		break;
	case LogLevel::Warning:
		if(verbosity < 2)
		{
			return;
		}
		break;
	case LogLevel::Error:
		if(verbosity < 1)
		{
			return;
		}
		break;
	}

	switch(level)
	{
	case LogLevel::Info:
		std::cout << "[Info]";
		break;
	case LogLevel::Warning:
		std::cout << "[Warning]";
		break;
	case LogLevel::Error:
		std::cout << "[Error]";
		break;
	}
	std::cout << " " << message << std::endl;
}

std::string version()
{
	std::ostringstream output;
	output << "DGValidator v" << VERSION << std::endl;
	return output.str();
}

std::string copyright()
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

std::string usage(const std::string& name, bool brief = false)
{
	std::ostringstream output;
	output <<
		"Usage: " << name << " [options] <drumkitfile>\n";
	if(!brief)
	{
		output <<
			"\n"
			"Validates the xml and semantics of the drumkit file and prints any found"
			" errors to the console.\n"
			"Returns 0 on success or 1 if errors were found.\n"
			"\n";
	}
	return output.str();
}

}

int main(int argc, char* argv[])
{
	bool no_audio{false};

	std::string hugin_filter;
	unsigned int hugin_flags = 0;
#ifndef DISABLE_HUGIN
	hugin_flags = HUG_FLAG_DEFAULT;
#endif /*DISABLE_HUGIN*/

	dg::Options opt;

	opt.add("no-audio", no_argument, 'n',
	        "Skip checking audio file existence and samplerate.",
	        [&]()
	        {
		        no_audio = true;
		        return 0;
	        });

	opt.add("verbose", no_argument, 'v',
	        "Print more info during validation. Can be added multiple times to"
	        " increase output verbosity.",
	        [&]()
	        {
		        ++verbosity;
		        return 0;
	        });

	opt.add("quiet", no_argument, 'q',
	        "Don't print any output, even on errors.",
	        [&]()
	        {
		        verbosity = 0;
		        return 0;
	        });

	opt.add("version", no_argument, 'V',
	        "Print version and exit.",
	        [&]()
	        {
		        std::cout << version();
		        exit(0);
		        return 0;
	        });

	opt.add("help", no_argument, 'h',
	        "Print this message and exit.",
	        [&]()
	        {
		        std::cout << usage(argv[0]);
		        std::cout << "Options:\n";
		        opt.help();
		        exit(0);
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

	if(opt.process(argc, argv) != 0)
	{
		return 1;
	}

	if(opt.arguments().empty())
	{
		std::cerr << "Missing kitfile." << std::endl;
		std::cerr << usage(argv[0], true) << std::endl;
		return 1;
	}

	hug_status_t status = hug_init(hugin_flags, HUG_OPTION_FILTER,
	                               hugin_filter.data(), HUG_OPTION_END);
	if(status != HUG_STATUS_OK)
	{
		std::cerr << "Error: " << status << std::endl;
		return 1;
	}

	std::string edited_filename = opt.arguments()[0];
	DrumkitDOM drumkitdom;
	std::vector<InstrumentDOM> instrumentdoms;
	std::string path = getPath(edited_filename);
	bool parseerror = false;
	bool ret = parseDrumkitFile(edited_filename, drumkitdom, logger);
	if(!ret)
	{
		WARN(drumkitloader, "Drumkit file parser error: '%s'",
		     edited_filename.data());
	}

	parseerror |= !ret;

	for(const auto& ref : drumkitdom.instruments)
	{
		instrumentdoms.emplace_back();
		bool ret = parseInstrumentFile(path + "/" + ref.file, instrumentdoms.back(),
		                               logger);
		if(!ret)
		{
			WARN(drumkitloader, "Instrument file parser error: '%s'",
			     edited_filename.data());
		}

		parseerror |= !ret;
	}

	if(parseerror)
	{
		return 1;
	}

	Settings settings;
	Random rand;
	DrumKit kit;

	DOMLoader domloader(settings, rand);
	ret = domloader.loadDom(path, drumkitdom, instrumentdoms, kit, logger);
	if(!ret)
	{
		WARN(drumkitloader, "DOMLoader error");
		logger(LogLevel::Error, "Validator found errors.");
		return 1;
	}
	parseerror |= !ret;
	if(parseerror)
	{
		ERR(drumgizmo, "Drumkit parser failed: %s\n", edited_filename.c_str());
		logger(LogLevel::Error, "Validator found errors.");
		return 1;
	}

	if(no_audio == false)
	{
		// Verify all referred audiofiles
		for(const auto& instrument: kit.instruments)
		{
			for(auto& audiofile: instrument->audiofiles)
			{
				audiofile->load(logger, 1);
				if(!audiofile->isLoaded())
				{
					WARN(drumkitloader, "Instrument file load error: '%s'",
					     audiofile->filename.data());
					logger(LogLevel::Warning, "Error loading audio file '" +
					       audiofile->filename + "' in the '" + instrument->getName() +
					       "' instrument");
					parseerror = true;
				}
			}
		}
	}

	if(parseerror)
	{
		logger(LogLevel::Warning, "Validator found errors.");
	}
	else
	{
		logger(LogLevel::Info, "Validator finished without errors.");
	}

	return parseerror ? 1 : 0;
}

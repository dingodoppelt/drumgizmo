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

void logger(LogLevel level, const std::string& message)
{
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

void printUsage(const char* prog, bool full = true)
{
	printf("Usage: %s <drumkit>|-h|--help\n", prog);
	if(!full)
	{
		return;
	}
	printf("Validates the xml and semantics of the drumkit file and prints "
	       "any found errors to the console.\n");
	printf("Returns 0 on success or 1 if errors were found.\n");
}

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		printUsage(argv[0], false);
		return 1;
	}

	if(std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")
	{
		printUsage(argv[0]);
		return 1;
	}

	std::string edited_filename = argv[1];
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

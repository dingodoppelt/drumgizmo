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
#include <climits>

#include <lodepng/lodepng.h>

#include <config.h>
#include <platform.h>

#if DG_PLATFORM != DG_PLATFORM_WINDOWS
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <image.h>

// Needed for Resource class
#include <resource_data.h>
const rc_data_t rc_data[] = {};

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

bool pathIsFile(const std::string& path)
{
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	return (GetFileAttributesA(path.data()) & FILE_ATTRIBUTE_DIRECTORY) == 0;
#else
	struct stat s;
	if(stat(path.data(), &s) != 0)
	{
		return false; // error
	}

	return (s.st_mode & S_IFREG) != 0; // s.st_mode & S_IFDIR => dir
#endif
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

	bool image_error{false};
	// Check drumkit images
	{
		if(!drumkitdom.metadata.image_map.empty() &&
		   drumkitdom.metadata.image.empty())
		{
			logger(LogLevel::Warning, "Found drumkit image_map but no image,"
			       " so image_map will not be usable.");
			image_error = true;
		}

		std::pair<std::size_t, std::size_t> image_size;
		if(!drumkitdom.metadata.image.empty())
		{
			// Check if the image file exists
			auto image = path + "/" + drumkitdom.metadata.image;
			logger(LogLevel::Info, "Found drumkit image '" + image + "'");
			if(!pathIsFile(image))
			{
				logger(LogLevel::Warning, "Image file does not exist.");
				image_error = true;
			}
			else
			{
				// Check if the image_map can be loaded (is a valid png file)
				GUI::Image img(image);
				if(!img.isValid())
				{
					logger(LogLevel::Warning, "Drumkit image, '" + image +
					       "', could not be loaded. Not a valid PNG image?");
					image_error = true;
				}
				else
				{
					image_size = { img.width(), img.height() };
					logger(LogLevel::Info, "Loaded image in resolution " +
					       std::to_string(image_size.first) + " x " +
					       std::to_string(image_size.second));
				}
			}
		}

		std::pair<std::size_t, std::size_t> image_map_size;
		if(!drumkitdom.metadata.image_map.empty())
		{
			// Check if the image_map file exists
			auto image_map = path + "/" + drumkitdom.metadata.image_map;
			logger(LogLevel::Info, "Found drumkit image_map '" + image_map + "'");
			if(!pathIsFile(image_map))
			{
				logger(LogLevel::Warning, "Image map file does not exist.");
				image_error = true;
			}
			else
			{
				// Check if the image_map can be loaded (is a valid png file)
				GUI::Image image(image_map);
				if(!image.isValid())
				{
					logger(LogLevel::Warning, "Drumkit image_map, '" + image_map +
					       "', could not be loaded. Not a valid PNG image?");
					image_error = true;
				}
				else
				{
					image_map_size = { image.width(), image.height() };
					logger(LogLevel::Info, "Loaded image_map in resolution " +
					       std::to_string(image_map_size.first) + " x " +
					       std::to_string(image_map_size.second));

					// Check if the click map colours can be found in the image_map image.
					for(const auto& clickmap : drumkitdom.metadata.clickmaps)
					{
						if(clickmap.colour.size() != 6)
						{
							logger(LogLevel::Warning,
							       "Clickmap colour field not the right length (should be 6).");
							image_error = true;
							continue;
						}

						try
						{
							auto hex_colour = std::stoul(clickmap.colour, nullptr, 16);
							float red   = (hex_colour >> 16 & 0xff) / 255.0f;
							float green = (hex_colour >>  8 & 0xff) / 255.0f;
							float blue  = (hex_colour >>  0 & 0xff) / 255.0f;
							GUI::Colour colour(red, green, blue);

							bool found{false};
							for(int y = 0; y < image.height() && !found; ++y)
							{
								for(int x = 0; x < image.width() && !found; ++x)
								{
									if(image.getPixel(x, y) == colour)
									{
										found = true;
									}
								}
							}

							if(!found)
							{
								logger(LogLevel::Warning,
								       "Clickmap colour '" + clickmap.colour +
								       "' not found in image_map.");
								image_error = true;
							}
						}
						catch(...)
						{
							// Not valid hex number
							logger(LogLevel::Warning,
							       "Clickmap colour not a valid hex colour.");
							image_error = true;
							continue;
						}

						// Check if the click map instruments exist.
						bool found{false};
						for(const auto& instrument: kit.instruments)
						{
							if(instrument->getName() == clickmap.instrument)
							{
								found = true;
							}
						}
						if(!found)
						{
							logger(LogLevel::Warning,
							       "Clickmap instrument '" + clickmap.instrument +
							       "' not found in drumkit.");
							image_error = true;
						}

					}
				}
			}
		}

		// Check if the image and the image_map have same resolutions
		if(image_size != image_map_size)
		{
			logger(LogLevel::Warning,
			       "Drumkit image and image_map does not have same resolution.");
			image_error = true;
		}

	}

	if(parseerror || image_error)
	{
		logger(LogLevel::Warning, "Validator found errors.");
		return 1;
	}

	logger(LogLevel::Info, "Validator finished without errors.");
	return 0;
}

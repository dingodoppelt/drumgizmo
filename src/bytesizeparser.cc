/* -*- Mode: c++ -*- */
/***************************************************************************
 *            bytesize_parser.cc
 *
 *  Sat Mar  4 18:00:12 CET 2017
 *  Copyright 2017 Goran Mekić
 *  meka@tilda.center
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
#include "bytesizeparser.h"
#include <iostream>
#include <stdexcept>


static std::size_t suffixToSize(const char& suffix)
{
	int size = 1;
	switch(suffix)
	{
	case 'k':
		size <<= 10;
		break;
	case 'M':
		size <<= 20;
		break;
	case 'G':
		size <<= 30;
		break;
	default:
		size = 0;
		break;
	}
	return size;
}


std::size_t byteSizeParser(const std::string& argument)
{
	std::string::size_type suffix_index;
	std::size_t stream_size;
	std::string suffix;
	bool error = false;

	if(argument.find('-') != std::string::npos)
	{
		error = true;
	}

	try
	{
		stream_size = std::stoi(argument, &suffix_index);
	}
	catch(std::invalid_argument)
	{
		std::cerr << "Invalid argument for diskstreamsize" << std::endl;
		error = true;
	}
	catch(std::out_of_range)
	{
		std::cerr << "Number too big. Try using bigger suffix for diskstreamsize" << std::endl;
		error = true;
	}
	if(!error)
	{
		suffix = argument.substr(suffix_index);
		if (suffix.length() > 1)
		{
			error = true;
		}
	}
	if(!error)
	{
		std::size_t suffix_size = 1;
		if(suffix_index <= suffix.length())
		{
			suffix_size = suffixToSize(suffix[0]);

		}
		stream_size *= suffix_size;
	}
	if(error)
	{
		return 0;
	}
	return stream_size;
}

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            compareoutputengine.cc
 *
 *  Sat May 14 13:27:04 CEST 2016
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
#include "compareoutputengine.h"

#include <iostream>

CompareOutputEngine::CompareOutputEngine()
	: AudioOutputEngine{}
	, info{}
	, file{"output"}
{
	info = {};
	info.samplerate = 44100;
	info.channels = 1;
	info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
}

CompareOutputEngine::~CompareOutputEngine()
{
	std::cout << "diff_samples: " << diff_samples << std::endl;
	sf_close(handle);
}

bool CompareOutputEngine::init(const Channels& channels)
{
	info.channels = channels.size();

	handle = sf_open(file.c_str(), SFM_READ, &info);
	if(handle == nullptr)
	{
		std::cerr << "[CompareOutputEngine] Failed to open "
		          << file << " for writing.\n";
		return false;
	}

	return true;
}

void CompareOutputEngine::setParm(const std::string& parm, const std::string& value)
{
	if(parm == "file")
	{
		// apply output filename prefix
		file = value;
	}
	else if(parm == "srate")
	{
		// try to apply samplerate
		try
		{
			info.samplerate = std::stoi(value);
		}
		catch(...)
		{
			std::cerr << "[CompareOutputEngine] Invalid samplerate " << value
			          << "\n";
		}
	}
	else
	{
		std::cerr << "[CompareOutputEngine] Unsupported parameter '" << parm
		          << "'\n";
	}
}

bool CompareOutputEngine::start()
{
	return true;
}

void CompareOutputEngine::stop()
{
}

void CompareOutputEngine::pre(size_t nsamples)
{
}

void CompareOutputEngine::run(int ch, sample_t* samples, size_t nsamples)
{
	if(ch >= info.channels)
	{
		std::cerr << "[CompareOutputEngine] cannot access channel #" << ch
		          << " (" << info.channels << " channels available)\n";
		return;
	}

	for(std::size_t i = 0; i < nsamples; ++i)
	{
		buffer[i * info.channels + ch] = samples[i];
	}
}

void CompareOutputEngine::post(size_t nsamples)
{
	nsamples = sf_readf_float(handle, ref_buffer, nsamples);
	if(nsamples == 0)
	{
		return;
	}

	for(std::size_t i = 0; i < nsamples; ++i)
	{
		for(std::size_t ch = 0; ch < (std::size_t)info.channels; ++ch)
		{
			if(buffer[i * info.channels + ch] != ref_buffer[i * info.channels + ch])
			{
				++diff_samples;

				// Use this to quit on first bad sample.
				//std::cerr << "ch: " << ch << ", pos: " << pos + i <<
				//	" expected: " << ref_buffer[i * info.channels + ch] <<
				//	" got: " << buffer[i * info.channels + ch] << std::endl;
				//exit(1);

			}
		}
	}
	pos += nsamples;
}

size_t CompareOutputEngine::getSamplerate() const
{
	return info.samplerate;
}

bool CompareOutputEngine::isFreewheeling() const
{
	return true;
}

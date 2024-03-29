/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            wavfile.cc
 *
 *  Mi 20. Jan 16:57:16 CET 2016
 *  Copyright 2016 Christian Gl�ckner
 *  cgloeckner@freenet.de
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

#include "wavfile.h"

WavfileOutputEngine::WavfileOutputEngine()
	: AudioOutputEngine{}
	, info{}
	, channels{}
	, file{"output"}
	, latency{0}
{
	info.samplerate = 44100;
	info.channels = 1;
	info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
}

WavfileOutputEngine::~WavfileOutputEngine()
{
	for(auto& ptr : channels)
	{
		if(ptr != nullptr)
		{
			sf_close(ptr);
		}
	}
}

bool WavfileOutputEngine::init(const Channels& data)
{
	channels.clear();
	channels.resize(data.size()); // value-initialized with null
	for(auto i = 0u; i < data.size(); ++i)
	{
		// write channel to file
		auto fname = file + data[i].name + "-" + std::to_string(i) + ".wav";
		channels[i] = sf_open(fname.c_str(), SFM_WRITE, &info);
		if(channels[i] == nullptr)
		{
			std::cerr << "[WaffileOutputEngine] Failed to initialize "
			          << "channel #" << i << "\n";
			return false;
		}
	}
	return true;
}

void WavfileOutputEngine::setParm(const std::string& parm,
                                  const std::string& value)
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
			std::cerr << "[WavfileOutputEngine] Invalid samplerate " << value
			          << "\n";
		}
	}
	else
	{
		std::cerr << "[WavfileOutputEngine] Unsupported parameter '" << parm
		          << "'\n";
	}
}

bool WavfileOutputEngine::start()
{
	return true;
}

void WavfileOutputEngine::stop()
{
}

void WavfileOutputEngine::pre(std::size_t nsamples)
{
}

void WavfileOutputEngine::run(int ch, sample_t* samples, std::size_t nsamples)
{
	if(static_cast<unsigned int>(ch) >= channels.size())
	{
		std::cerr << "[WavfileOutputEngine] cannot access channel #" << ch
		          << " (" << channels.size() << " channels available)\n";
		return;
	}

	// Skip the initial 'latency' samples.
	if(nsamples <= latency)
	{
		return;
	}

	nsamples -= latency;

	sf_writef_float(channels[ch], samples + latency, nsamples);
}

void WavfileOutputEngine::post(std::size_t nsamples)
{
	if(latency > nsamples)
	{
		latency -= nsamples;
	}
	else
	{
		latency = 0;
	}
}

std::size_t WavfileOutputEngine::getSamplerate() const
{
	return info.samplerate;
}

void WavfileOutputEngine::onLatencyChange(std::size_t latency)
{
	this->latency = latency;
}

bool WavfileOutputEngine::isFreewheeling() const
{
	return true;
}

/* -*- Mode: c++ -*- */
/***************************************************************************
 *            oss.cc
 *
 *  Tue Apr 11 19:42:45 CET 2017
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
#include "oss.h"
#include <sys/soundcard.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>


OSSOutputEngine::OSSOutputEngine()
	: dev{"/dev/dsp"}
	, num_channels{16}
	, srate{44100}
	, format{AFMT_S32_NE}
	, data{}
	, max_fragments{4}
	, fragment_size{8}
	, buffer_size{1024}
{
	data.clear();
	data.resize(1024 * num_channels);
}

bool OSSOutputEngine::init(const Channels& channels)
{
	int tmp, mode = O_WRONLY, fragments;
	num_channels = channels.size();

	if((fd = open(dev.data(), mode, 0)) == -1)
	{
		std::cerr << dev.data() << ' ' << std::strerror(errno) << std::endl;
		return false;
	}

	fragments = max_fragments << 16 | fragment_size;
	tmp = fragments;
	if (ioctl(fd, SNDCTL_DSP_SETFRAGMENT, &tmp) == -1 || tmp != fragments)
	{
		perror("SNDCTL_DSP_SETFRAGMENT");
		exit(-1);
	}

	audio_buf_info info;
	if(ioctl(fd, SNDCTL_DSP_GETOSPACE, &info) == -1)
	{
		std::cerr << "Can not get buffer info: ";
		std::cerr << std::strerror(errno) << std::endl;
		return false;
	}
	buffer_size = info.bytes / 4;

	tmp = format;
	if(ioctl(fd, SNDCTL_DSP_SETFMT, &tmp) == -1 || tmp != format)
	{
		std::cerr << "Setting audio format failed " << std::strerror(errno);
		std::cerr << std::endl;
		return false;
	}

	tmp = num_channels;
	if(ioctl(fd, SNDCTL_DSP_CHANNELS, &tmp) == -1 || tmp != num_channels)
	{
		std::cerr << "Can not set number of channels to " << num_channels;
		std::cerr << ": " << std::strerror(errno) << std::endl;
		return false;
	}

	tmp = srate;
	if(ioctl(fd, SNDCTL_DSP_SPEED, &tmp) == -1 || tmp != srate)
	{
		std::cerr << "Can not set sampling frequency to " << srate << ": ";
		std::cerr << std::strerror(errno) << std::endl;
		return false;
	}

	return true;
}

void OSSOutputEngine::setParm(const std::string& parm, const std::string& value)
{
	if(parm == "dev")
	{
		dev = value;
	}
	else if(parm == "srate")
	{
		try
		{
			srate = std::stoi(value);
		}
		catch(...)
		{
			std::cerr << "[OSSOutputEngine] Invalid samplerate ";
			std::cerr << value << std::endl;
		}
	}
	else if(parm == "max_fragments")
	{
		try
		{
			max_fragments = std::stoi(value);
		}
		catch(...)
		{
			std::cerr << "[OSSOutputEngine] Invalid max_fragments ";
			std::cerr << value << std::endl;
		}
		if(max_fragments < 2)
		{
			std::cerr << "[OSSoutputEngine] max_fragments must be at least 2\n";
			std::cerr << "Setting max_fragments to 2" << std::endl;
			max_fragments = 2;
		}
	}
	else if(parm == "fragment_size")
	{
		try
		{
			fragment_size = std::stoi(value);
		}
		catch(...)
		{
			std::cerr << "[OSSOutputEngine] Invalid fragment_size ";
			std::cerr << value << std::endl;
		}
		if(fragment_size < 4)
		{
			std::cerr << "[OSSoutputEngine] fragment_size must be at least 4\n";
			std::cerr << "Setting fragment_size to 4 (";
			fragment_size = 4;
			auto fragment_byte_size = 1 << fragment_size;
			std::cerr << fragment_byte_size << "Bytes)" << std::endl;
		}
	}
	else
	{
		std::cerr << "[OSSOutputEngine] Unsupported parameter '";
		std::cerr << parm << std::endl;
	}
}

void OSSOutputEngine::pre(size_t nsamples)
{
	data.resize(nsamples * num_channels);
}

void OSSOutputEngine::run(int ch, sample_t* samples, size_t nsamples)
{
	// Convert to int format and
	// write channel data in interleaved buffer
	std::int32_t sample;
	for(size_t i = 0; i < nsamples; ++i)
	{
		// Hard clip if needed
		if(samples[i] > 1)
		{
			sample = INT32_MAX;
		}
		else if(samples[i] < -1)
		{
			sample = -INT32_MAX;
		}
		else
		{
			sample = samples[i] * INT32_MAX;
		}
		data[i * num_channels + ch] = sample;
	}
}

void OSSOutputEngine::post(size_t nsamples)
{
	auto data_size = data.size() * sizeof(*data.data());
	auto size_written = write(fd, data.data(), data_size);
	if(size_written != data_size)
	{
		std::cerr << "Audio write: " << std::strerror(errno) << std::endl;
	}
}

std::size_t OSSOutputEngine::getBufferSize() const
{
	return buffer_size;
}

std::size_t OSSOutputEngine::getSamplerate() const
{
	return srate;
}

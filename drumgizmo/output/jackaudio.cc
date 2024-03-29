/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            jackaudio.cc
 *
 *  Fr 22. Jan 09:43:30 CET 2016
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
#include <cassert>
#include <iostream>
#include <cstring>

#include "jackaudio.h"

JackAudioOutputEngine::JackAudioOutputEngine(JackClient& client)
	: AudioOutputEngine{}
	, client(client)
	, channels{}
	, latency{0}
{
	client.add(*this);
}

JackAudioOutputEngine::~JackAudioOutputEngine()
{
	client.remove(*this);
}

bool JackAudioOutputEngine::init(const Channels& data)
{
	channels.clear();
	channels.reserve(data.size());
	auto i = 0u;
	auto const buffer_size = getBufferSize();

	for(auto const& elem : data)
	{
		auto name = std::to_string(i) + "-" + elem.name;
		// initialize new channel
		channels.emplace_back(client, name, buffer_size);

		if(channels.back().port.port == nullptr)
		{
			std::cerr << "[JackAudioOutputEngine] Cannot create jack "
			          << "port for channel #" << i << "\n";
			return false;
		}
		++i;
	}
	return true;
}

void JackAudioOutputEngine::setParm(const std::string& parm, const std::string& value)
{
}

bool JackAudioOutputEngine::start()
{
	client.activate();
	return true;
}

void JackAudioOutputEngine::stop()
{
}

void JackAudioOutputEngine::pre(size_t nsamples)
{
	// Clear all channels
	for(auto& channel : channels)
	{
		assert(channel.samples.size() == nsamples);
		std::memset(channel.samples.data(), 0, nsamples * sizeof(sample_t));
	}
}

void JackAudioOutputEngine::run(int ch, sample_t* samples, size_t nsamples)
{
	for(auto i = 0u; i < nsamples; ++i)
	{
		channels[ch].samples[i] = samples[i];
	}
}

void JackAudioOutputEngine::post(size_t nsamples)
{
	sema.wait();
}

void JackAudioOutputEngine::process(jack_nframes_t num_frames)
{
	assert(num_frames == getBufferSize());

	for(auto& channel : channels)
	{
		auto ptr = static_cast<jack_default_audio_sample_t*>(jack_port_get_buffer(channel.port.port, num_frames));
		for(auto i = 0u; i < num_frames; ++i)
		{
			ptr[i] = channel.samples[i];
		}
	}
	sema.post();
}

size_t JackAudioOutputEngine::getBufferSize() const
{
	return client.getBufferSize();
}

size_t JackAudioOutputEngine::getSamplerate() const
{
	return client.getSampleRate();
}

bool JackAudioOutputEngine::isFreewheeling() const
{
	return client.isFreewheeling();
}

void JackAudioOutputEngine::onLatencyChange(std::size_t latency)
{
	this->latency = latency;
}

JackAudioOutputEngine::Channel::Channel(JackClient& client,
                                        const std::string& name,
                                        std::size_t buffer_size)
	: port{client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput}
	, samples{}
{
	samples.resize(buffer_size);
}

void JackAudioOutputEngine::jackLatencyCallback(jack_latency_callback_mode_t mode)
{
	jack_latency_range_t range;
	switch(mode)
	{
	case JackCaptureLatency:
		// We do not have any audio input ports. Use this for when we do...
//		jack_port_get_latency_range(port_feeding_input_port,
//		                            JackPlaybackLatency,
//		                            &range);
//		range.min += 0;
//		range.max += 0;
//		jack_port_set_latency_range(input_port, JackPlaybackLatency, &range);
		break;
	case JackPlaybackLatency:
		for(auto& channel : channels)
		{
			jack_port_get_latency_range(channel.port.port,
			                            JackPlaybackLatency,
			                            &range);
			range.min += latency;
			range.max += latency;
			jack_port_set_latency_range(channel.port.port,
			                            JackPlaybackLatency,
			                            &range);
		}
		break;
	}
}

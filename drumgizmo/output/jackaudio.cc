/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            jackaudio.cc
 *
 *  Fr 22. Jan 09:43:30 CET 2016
 *  Copyright 2016 Christian Glöckner
 *  cgloeckner@freenet.de
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include <cassert>
#include <iostream>

#include "jackaudio.h"

JackaudioOutputEngine::JackaudioOutputEngine(JackClient& client)
	: client{client}
	, channels{}
	, sema("jackaudio") {
	client.add(*this);
}

JackaudioOutputEngine::~JackaudioOutputEngine() {
}

bool JackaudioOutputEngine::init(Channels data) {
	channels.clear();
	channels.reserve(data.size());
	auto i = 0u;
	auto const buffer_size = getBufferSize();
	
	for (auto const & elem: data) {
		auto name = std::to_string(i) + "-" + elem.name;
		// initialize new channel
		channels.emplace_back(client, buffer_size, name);
		
		if (channels.back().port == nullptr) {
			std::cerr << "[JackaudioOutputEngine] Cannot create jack "
				<< "port for channel #" << i << "\n";
			return false;
		}
		++i;
	}
	return true;
}

void JackaudioOutputEngine::setParm(std::string parm, std::string value) {
}

bool JackaudioOutputEngine::start() {
	client.activate();
	return true;
}

void JackaudioOutputEngine::stop() {
}

void JackaudioOutputEngine::pre(size_t nsamples) {
}

void JackaudioOutputEngine::run(int ch, sample_t* samples, size_t nsamples) {
	for (auto i = 0u; i < nsamples; ++i) {
		channels[ch].samples[i] = samples[i];
	}
}

void JackaudioOutputEngine::post(size_t nsamples) {
	sema.wait();
}

void JackaudioOutputEngine::process(jack_nframes_t num_frames) {
	assert(num_frames == getBufferSize());
	
	for (auto& channel: channels) {
		auto ptr = static_cast<jack_default_audio_sample_t*>(
			jack_port_get_buffer(channel.port, num_frames));
		for (auto i = 0u; i < num_frames; ++i) {
			ptr[i] = channel.samples[i];
		}
	}
	sema.post();
}

size_t JackaudioOutputEngine::getBufferSize() {
	return client.getBufferSize();
}

size_t JackaudioOutputEngine::samplerate() {
	return client.getSampleRate();
}

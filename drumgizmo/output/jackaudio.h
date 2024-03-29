/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            jackaudio.h
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
#pragma once
#include <vector>
#include <sem.h>

#include "audiooutputengine.h"
#include "../jackclient.h"

class JackAudioOutputEngine
	: public AudioOutputEngine
	, public JackProcess
{
public:
	JackAudioOutputEngine(JackClient& client);
	~JackAudioOutputEngine();

	// based on AudioOutputEngine
	bool init(const Channels& chan) override;
	void setParm(const std::string& parm, const std::string& value) override;
	bool start() override;
	void stop() override;
	void pre(size_t nsamples) override;
	void run(int ch, sample_t* samples, size_t nsamples) override;
	void post(size_t nsamples) override;
	size_t getBufferSize() const override;
	size_t getSamplerate() const override;
	bool isFreewheeling() const override;
	void onLatencyChange(std::size_t latency) override;

	// based on JackProcess
	void process(jack_nframes_t num_frames) override;
	void jackLatencyCallback(jack_latency_callback_mode_t mode) override;

private:

	struct Channel
	{
		JackPort port;
		std::vector<sample_t> samples;

		Channel(JackClient& client, const std::string& name,
		        std::size_t buffer_size);
	};

	JackClient& client;
	std::vector<Channel> channels;
	Semaphore sema;
	std::size_t latency{0};
};

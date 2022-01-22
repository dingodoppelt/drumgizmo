/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            jackclient.h
 *
 *  Sun Jul 20 21:48:44 CEST 2008
 *  Copyright 2008 Bent Bisballe Nyeng
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
#pragma once
#include <vector>
#include <string>
#include <list>

#include <jack/jack.h>

#include <audiotypes.h>

class JackClient;

class JackProcess
{
public:
	virtual ~JackProcess() = default;
	virtual void process(jack_nframes_t num_frames) = 0;
	virtual void jackLatencyCallback(jack_latency_callback_mode_t mode) {}
};

// --------------------------------------------------------------------

// RAII-wrapper for jack_port_t
struct JackPort
{
	JackPort(JackClient& client, const std::string& name, const char* type,
	         JackPortFlags flags);
	~JackPort();

	jack_client_t* const client;
	jack_port_t* const port;
};

// --------------------------------------------------------------------

class JackClient
{
	friend struct JackPort;

public:
	JackClient();
	~JackClient();

	void add(JackProcess& process);
	void remove(JackProcess& process);
	void activate();

	std::size_t getBufferSize() const;
	std::size_t getSampleRate() const;
	bool isFreewheeling() const;

private:
	jack_client_t* client;
	bool dirty{false};
	struct JackProcessContainer
	{
		JackProcess *process;
		bool active{true};
	};
	std::list<JackProcessContainer> processes;
	bool is_active;
	bool is_freewheeling;

	int process(jack_nframes_t num_frames);
	void jackLatencyCallback(jack_latency_callback_mode_t mode);
	void jackFreewheelCallback(bool is_freewheeling);

	static int wrapJackProcess(jack_nframes_t nframes, void* arg);
	static void latencyCallback(jack_latency_callback_mode_t mode, void* arg);
	static void freewheelCallback(int is_freewheeling, void* arg);
};

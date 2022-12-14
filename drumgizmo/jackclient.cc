/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            jackclient.cc
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
#include <cassert>

#include "jackclient.h"

JackPort::JackPort(JackClient& client, const std::string& name,
                   const char* type, JackPortFlags flags)
	: client{client.client} // register jack port for given client
	, port{jack_port_register(this->client, name.c_str(), type, flags, 0)}
{
}

JackPort::~JackPort()
{
	if(port != nullptr)
	{
		assert(client != nullptr);
		jack_port_unregister(client, port);
	}
}

// --------------------------------------------------------------------

int JackClient::wrapJackProcess(jack_nframes_t nframes, void* arg)
{
	return static_cast<JackClient*>(arg)->process(nframes);
}

void JackClient::latencyCallback(jack_latency_callback_mode_t mode, void* arg)
{
	static_cast<JackClient*>(arg)->jackLatencyCallback(mode);
}

void JackClient::freewheelCallback(int is_freewheeling, void* arg)
{
	static_cast<JackClient*>(arg)->jackFreewheelCallback(is_freewheeling);
}

JackClient::JackClient()
	: client{nullptr}
	, processes{}
	, is_active{false}
{
	jack_status_t status;
	client = jack_client_open("DrumGizmo", JackNullOption, &status);

	// Register callbacks
	jack_set_process_callback(client, JackClient::wrapJackProcess, this);
	jack_set_latency_callback(client, JackClient::latencyCallback, this);
	jack_set_freewheel_callback(client, JackClient::freewheelCallback, this);
}

JackClient::~JackClient()
{
	if(client != nullptr)
	{
		jack_client_close(client);
	}
}

void JackClient::add(JackProcess& process)
{
	JackProcessContainer c;
	c.process = &process;
	processes.push_back(std::move(c));
}

void JackClient::remove(JackProcess& process)
{
	// Do not erase here. Instead mark as disabled - it will be erased at next
	// JackClient::process call.
	for(auto& ptr : processes)
	{
		if(ptr.process == &process)
		{
			ptr.active = false;
		}
	}
	dirty = true;
}

void JackClient::activate()
{
	if(!is_active)
	{
		jack_activate(client);
	}
	is_active = true;
}

int JackClient::process(jack_nframes_t num_frames)
{
	// Clear out any inactive processes before iterating
	if(dirty)
	{
		auto it = processes.begin();
		while(it != processes.end())
		{
			if(it->active == false)
			{
				it = processes.erase(it);
			}
			else
			{
				it++;
			}
		}

		dirty = false;
	}

	for(auto& ptr : processes)
	{
		ptr.process->process(num_frames);
	}

	return 0;
}

void JackClient::jackLatencyCallback(jack_latency_callback_mode_t mode)
{
	for(auto& ptr : processes)
	{
		ptr.process->jackLatencyCallback(mode);
	}
}

void JackClient::jackFreewheelCallback(bool is_freewheeling)
{
	this->is_freewheeling = is_freewheeling;
}

std::size_t JackClient::getBufferSize() const
{
	return jack_get_buffer_size(client);
}

std::size_t JackClient::getSampleRate() const
{
	return jack_get_sample_rate(client);
}

bool JackClient::isFreewheeling() const
{
	return is_freewheeling;
}

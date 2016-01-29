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

#include "jackclient.h"

JackProcess::~JackProcess()
{
}

// --------------------------------------------------------------------

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

static int _wrap_jack_process(jack_nframes_t nframes, void* arg)
{
	return static_cast<JackClient*>(arg)->process(nframes);
}

JackClient::JackClient() : client{nullptr}, processes{}, is_active{false}
{
	jack_status_t status;
	client = jack_client_open("DrumGizmo", JackNullOption, &status);
	jack_set_process_callback(client, _wrap_jack_process, this);
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
	processes.insert(&process);
}

void JackClient::remove(JackProcess& process)
{
	processes.erase(&process);
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
	for(auto& ptr : processes)
	{
		ptr->process(num_frames);
	}
	return 0;
}

std::size_t JackClient::getBufferSize() const
{
	return jack_get_buffer_size(client);
}

std::size_t JackClient::getSampleRate() const
{
	return jack_get_sample_rate(client);
}

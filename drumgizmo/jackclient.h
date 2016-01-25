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
#pragma once
#include <vector>
#include <string>
#include <set>

#include <jack/jack.h>

#include <audiotypes.h>

class JackClient;

class JackProcess {
	public:
		virtual ~JackProcess();
		virtual void process(jack_nframes_t num_frames) = 0;
};

// --------------------------------------------------------------------

// RAII-wrapper for jack_port_t
struct JackPort {
	JackPort(JackClient& client, std::string const & name, const char * type, JackPortFlags flags);
	~JackPort();
	
	jack_client_t* const client;
	jack_port_t* const port;
};

// --------------------------------------------------------------------

class JackClient {
	friend struct JackPort;
	
	public:
		JackClient();
		~JackClient();
		
		void add(JackProcess& process);
		void remove(JackProcess& process);
		void activate();
		int process(jack_nframes_t num_frames);
		std::size_t getBufferSize() const;
		std::size_t getSampleRate() const;
		
	private:
		jack_client_t* client;
		std::set<JackProcess*> processes;
		bool is_active;
};

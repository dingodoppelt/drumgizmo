/* -*- Mode: c++ -*- */
/***************************************************************************
 *            ossmidi.cc
 *
 *  Sun May 21 10:52:09 CEST 2017
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
#include "ossmidi.h"
#include <sys/soundcard.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>


OSSInputEngine::OSSInputEngine()
	: AudioInputEngineMidi{}
	, dev{"/dev/midi"}
{
}


OSSInputEngine::~OSSInputEngine()
{
}


bool OSSInputEngine::init(const Instruments& instruments)
{
	if(!loadMidiMap(midimap_file, instruments))
	{
		std::cerr << "[OSSInputEngine] Failed to parse midimap '"
		          << midimap_file << std::endl;
		return false;
	}
	if ((fd = open(dev.data(), O_RDONLY | O_NONBLOCK, 0)) == -1)
	{
		std::cerr << dev.data() << ' ' << std::strerror(errno) << std::endl;
		return false;
	}
	return true;
}


void OSSInputEngine::setParm(const std::string& parm, const std::string& value)
{
	if(parm == "dev")
	{
		dev = value;
	}
	else if(parm == "midimap")
	{
		midimap_file = value;
	}
}


bool OSSInputEngine::start()
{
	return true;
}

void OSSInputEngine::stop()
{
}


void OSSInputEngine::pre()
{
	events.clear();
}


void OSSInputEngine::run(size_t pos, size_t len, std::vector<event_t>& events)
{
	int l;
	unsigned char buf[128];
	if ((l = read (fd, buf, sizeof (buf))) != -1)
	{
		processNote(buf, l,
		            0, // No time information available? play as soon as possible
		            events);
	}
	else if (errno != EAGAIN)
	{
		std::cerr << "Error code: " << errno << std::endl;
		std::cerr << std::strerror(errno) << std::endl;
	}
}


void OSSInputEngine::post()
{
}


bool OSSInputEngine::isFreewheeling() const
{
	return false;
}

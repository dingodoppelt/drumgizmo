/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            channel.h
 *
 *  Tue Jul 22 17:14:27 CEST 2008
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

#include "audio.h"

#define ALL_CHANNELS ((channel_t)0xffffffff)
#define NO_CHANNEL ((channel_t)0xfffffffe)

enum class main_state_t
{
	unset,
	is_main,
	is_not_main
};

class Channel
{
public:
	Channel(const std::string& name = "");

	std::string name;
	channel_t num;
};

class InstrumentChannel
	: public Channel
{
public:
	InstrumentChannel(const std::string& name = "");

	main_state_t main{main_state_t::unset};
};

typedef std::vector<Channel> Channels;

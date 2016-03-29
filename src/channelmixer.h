/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            channelmixer.h
 *
 *  Sun Oct 17 10:14:34 CEST 2010
 *  Copyright 2010 Bent Bisballe Nyeng
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

#include <map>

#include "channel.h"

class MixerSettings
{
public:
	float gain;
	const Channel* output;
};

class ChannelMixer
{
public:
	ChannelMixer(const Channels& channels,
	    const Channel* defaultchannel = nullptr, float defaultgain = 1.0);

	MixerSettings& lookup(const InstrumentChannel& channel);

	void setDefaults(const Channel* defaultchannel, float defaultgain);

private:
	std::map<const InstrumentChannel*, MixerSettings> mix;

	const Channel* defaultchannel;
	float defaultgain;

	const Channels& channels;
};

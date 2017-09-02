/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            sample.cc
 *
 *  Mon Jul 21 10:23:20 CEST 2008
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
#include "sample.h"

#include <sndfile.h>

Sample::Sample(const std::string& name, float power)
	: name{name}
	, power{power}
	, audiofiles{}
{
}

Sample::~Sample()
{
}

void Sample::addAudioFile(InstrumentChannel* c, AudioFile* a)
{
	audiofiles[c] = a;
}

AudioFile* Sample::getAudioFile(const Channel& channel)
{
	/*
	if(audiofiles.find(c) == audiofiles.end()) return nullptr;
	return audiofiles[c];
	*/

	// todo: std::find_if ??
	for (auto& pair: audiofiles)
	{
		if (pair.first->num == channel.num)
		{
			return pair.second;
		}
	}

	return nullptr;
}

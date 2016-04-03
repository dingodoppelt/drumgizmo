/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            test.cc
 *
 *  Sun Apr  3 13:21:26 CEST 2016
 *  Copyright 2016 Jonas Suhr Christensen
 *  jsc@umbraculum.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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
#include <stdlib.h>
#include <string>

#include "event.h"
#include "test.h"

TestInputEngine::TestInputEngine()
	: AudioInputEngine{}
	, probability{0.1}
	, instrument{-1}
	, length{-1}
{
}

TestInputEngine::~TestInputEngine()
{
}

bool TestInputEngine::init(Instruments& instruments)
{
  return true;
}

void TestInputEngine::setParm(std::string parm, std::string value)
{
/*
	if(parm == "probability")
	{
  		probability = atof(value.c_str());
	}
	if(parm == "instrument")
	{
		instrument = atoi(value.c_str());
	}
	if(parm == "length")
	{
		length = atoi(value.c_str());
	}
*/	
}

bool TestInputEngine::start()
{
  return true;
}

void TestInputEngine::stop()
{
}

void TestInputEngine::pre()
{
}

void TestInputEngine::run(size_t pos, size_t len, std::vector<event_t>& events)
{
/*
	if((float)rand() / (float)RAND_MAX > probability) 
	{
		return;
	}

	events.emplace_back();
	auto& event = events.back();
	event.type = TYPE_ONSET;

	if(length != -1 && (int)pos > length * 44100) 
	{
		event.type = TYPE_STOP;
	}
	else 
	{
		event.type = TYPE_ONSET;
	}


	if(instrument != -1) 
	{
		event.instrument = instrument;
	}
	else
	{
		event.instrument = rand() % 32;
	}

	event.velocity = (float)rand()/(float)RAND_MAX;
	event.offset = len;
*/
}

void TestInputEngine::post()
{
}

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audiocacheeventhandlertest.cc
 *
 *  Thu Jan  7 15:44:14 CET 2016
 *  Copyright 2016 Bent Bisballe Nyeng
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
#include "dgunit.h"

#include <audiocacheeventhandler.h>

class AudioCacheEventHandlerTest
	: public DGUnit
{
public:
	AudioCacheEventHandlerTest()
	{
		DGUNIT_TEST(AudioCacheEventHandlerTest::threadedTest);
	}

public:
	void setup() override {}
	void teardown() override {}

	void threadedTest()
	{
		AudioCacheIDManager id_manager;
		id_manager.init(10);

		AudioCacheEventHandler event_handler(id_manager);
	}
};

// Registers the fixture into the 'registry'
static AudioCacheEventHandlerTest test;

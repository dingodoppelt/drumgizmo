/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            testmain.cc
 *
 *  Sun Nov 22 20:06:42 CET 2015
 *  Copyright 2015 Bent Bisballe Nyeng
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
#include "plugingui.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <hugin.hpp>

#include <notifier.h>
#include <settings.h>

class TestMain : public Listener {
public:
	TestMain()
	{
		CONNECT(&gui, closeNotifier, this, &TestMain::stop);
	}

	void stop()
	{
		DEBUG(stop, "Stopping...\n");
		running = false;
	}

	void run()
	{
		while(running)
		{
#ifdef WIN32
			SleepEx(50, FALSE);
#else
			usleep(50000);
#endif
			gui.processEvents();
		}
	}

	bool running = true;

	Settings settings;
	GUI::PluginGUI gui{settings};
};

int main()
{
	INFO(example, "We are up and running");

	TestMain testMain;
	testMain.run();

	return 0;
}

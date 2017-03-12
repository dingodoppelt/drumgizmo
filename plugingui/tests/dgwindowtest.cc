/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            dgwindowtest.cc
 *
 *  Fri Feb 17 16:25:32 CET 2017
 *  Copyright 2017 Bent Bisballe Nyeng
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
#include <platform.h>

#if DG_PLATFORM == DG_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <unistd.h>

#include <hugin.hpp>
#include <settings.h>

#include <dgwindow.h>
#include <eventhandler.h>
#include <pluginconfig.h>

int main()
{
	INFO(example, "We are up and running");

	Settings settings;
	SettingsNotifier settings_notifier{settings};
	GUI::Config config;
	GUI::DGWindow main_window(nullptr, config, settings, settings_notifier);
	main_window.show();

	bool running = true;
	while(running)
	{
		settings_notifier.evaluate();
		main_window.eventHandler()->processEvents();
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
		SleepEx(50, FALSE);
#else
		usleep(50000);
#endif
	}

	return 0;
}

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
#include <platform.h>

#if DG_PLATFORM == DG_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <unistd.h>

#include <hugin.hpp>
#include <settings.h>

#include "window.h"
#include "mainwindow.h"

int main()
{
	INFO(example, "We are up and running");

	GUI::Window parent{nullptr};
	parent.setCaption("PluginGui Test Application");

	Settings settings;
	GUI::MainWindow main_window(settings, parent.getNativeWindowHandle());
	CONNECT(&parent, eventHandler()->closeNotifier,
			&main_window, &GUI::MainWindow::closeEventHandler);

	parent.show();
	main_window.show();

	parent.resize(370, 330);

	while(true)
	{
		parent.eventHandler()->processEvents();
		if(!main_window.processEvents())
		{
			break;
		}

#if DG_PLATFORM == DG_PLATFORM_WINDOWS
		SleepEx(50, FALSE);
#else
		usleep(50000);
#endif
	}

	return 0;
}

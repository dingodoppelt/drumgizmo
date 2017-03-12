/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            plugingui.cc
 *
 *  Mon Oct  3 13:40:49 CEST 2011
 *  Copyright 2011 Bent Bisballe Nyeng
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

#include <iostream>

#include <hugin.hpp>

#include "pluginconfig.h"

namespace GUI {

PluginGUI::PluginGUI(Settings& settings, void* native_window)
	: native_window(native_window)
	, settings(settings)
{
	init();
}

PluginGUI::~PluginGUI()
{
	deinit();
}

bool PluginGUI::processEvents()
{
	if(!initialised)
	{
		return running;
	}

	window->eventHandler()->processEvents();

	{
		Painter p(*window);

		settings_notifier.evaluate();
	}

	if(closing)
	{
		closeNotifier();
		closing = false;
		return false;
	}

	return running;
}

void PluginGUI::init()
{
	DEBUG(gui, "init");

	config = new Config();
	config->load();

	window = new DGWindow(native_window, *config, settings, settings_notifier);

	auto eventHandler = window->eventHandler();
	CONNECT(eventHandler, closeNotifier, this, &PluginGUI::closeEventHandler);

	window->show();

	initialised = true;
}

void PluginGUI::deinit()
{
	if(config)
	{
		config->save();
		delete config;
	}

	if(window)
	{
		delete window;
	}
}

void PluginGUI::show()
{
	while(!initialised)
	{
		usleep(10000);
	}

	if(!window)
	{
		init();
	}

	if(window)
	{
		window->show();
	}
}

void PluginGUI::hide()
{
	while(!initialised)
	{
		usleep(10000);
	}

	if(window)
	{
		window->hide();
	}
}

void PluginGUI::setWindowClosedCallback(void (*handler)(void *), void* ptr)
{
	windowClosedHandler = handler;
	windowClosedPtr = ptr;
}

void PluginGUI::closeEventHandler()
{
	closing = true;
	closeNotifier();
	// Call old-style notifier if one is registered.
	if(windowClosedHandler)
	{
		windowClosedHandler(windowClosedPtr);
	}
}

} // GUI::

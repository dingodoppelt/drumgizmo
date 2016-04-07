/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            plugingui.h
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
#pragma once

#include <settings.h>
#include <notifier.h>

#include "dgwindow.h"
#include "eventhandler.h"
#include "pluginconfig.h"

namespace GUI {

class PluginGUI
	: public Listener
{
public:
	PluginGUI(Settings& settings, void* native_window = nullptr);
	virtual ~PluginGUI();

	//! Process all events and messages in queue
	//! \return true if not closing, returns false if closing.
	bool processEvents();

	void init();
	void deinit();

	void show();
	void hide();

	DGWindow* window{nullptr};
	EventHandler* eventhandler{nullptr};

	Config* config{nullptr};

	Notifier<> closeNotifier;

	// Support old interface a little while longer..
	void setWindowClosedCallback(void (*handler)(void*), void* ptr);

private:
	void closeEventHandler();

	void* native_window{nullptr};

	volatile bool running{true};
	volatile bool closing{false};
	volatile bool initialised{false};

	// For the old-style notifier.
	void (*windowClosedHandler)(void *){nullptr};
	void *windowClosedPtr{nullptr};

	Settings& settings;
	SettingsNotifier settings_notifier{settings};
};

} // GUI::

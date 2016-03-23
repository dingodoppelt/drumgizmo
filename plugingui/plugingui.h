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
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#pragma once

#include "dgwindow.h"
#include "eventhandler.h"

#include "pluginconfig.h"

#include <settings.h>

#include "messagereceiver.h"
#include "notifier.h"

namespace GUI {

class PluginGUI
	: public MessageReceiver
	, public Listener
{
public:
	PluginGUI(void* native_window = nullptr);
	virtual ~PluginGUI();

	//! Process all events and messages in queue
	//! \return true if not closing, returns false if closing.
	bool processEvents();

	void init();
	void deinit();

	void show();
	void hide();


	void handleMessage(Message* msg);

	DGWindow* window{nullptr};
	EventHandler* eventhandler{nullptr};

	Config* config{nullptr};

	Notifier<> closeNotifier;

	// Setting notifiers:
	Notifier<bool> enable_velocity_modifier_notifier;
	Notifier<float> velocity_modifier_falloff_notifier;
	Notifier<float> velocity_modifier_weight_notifier;

	Notifier<bool> enable_velocity_randomiser_notifier;
	Notifier<float> velocity_randomiser_weight_notifier;

	Notifier<double> samplerate_notifier;

	Notifier<bool> enable_resampling_notifier;

	Notifier<float> drumkit_file_progress_notifier;
	//Notifier<std::string> current_file_notifier;

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

	Settings settings;
	SettingsGetter getter{settings};
};

} // GUI::

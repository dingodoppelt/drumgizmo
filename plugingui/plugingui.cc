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
#include "plugingui.h"

#include <hugin.hpp>

#include "pluginconfig.h"
#include "messagehandler.h"

#include <iostream>

namespace GUI {

PluginGUI::PluginGUI(void* native_window)
	: MessageReceiver(MSGRCV_UI)
	, native_window(native_window)
{
	init();
}

PluginGUI::~PluginGUI()
{
}

void PluginGUI::handleMessage(Message *msg)
{
	Painter p(*window);// Make sure we only redraw buffer once (set refcount to 1)

	switch(msg->type()) {
	case Message::LoadStatus:
		{
			LoadStatusMessage *ls = (LoadStatusMessage*)msg;
			window->drumkitFileProgress->setProgress((float)ls->numer_of_files_loaded /
			                              (float)ls->number_of_files);
			if(ls->numer_of_files_loaded == ls->number_of_files)
			{
				window->drumkitFileProgress->setState(ProgressBarState::Green);
			}
		}
		break;
	case Message::LoadStatusMidimap:
		{
			LoadStatusMessageMidimap *ls = (LoadStatusMessageMidimap*)msg;
			window->midimapFileProgress->setProgress(1);
			if(ls->success)
			{
				window->midimapFileProgress->setState(ProgressBarState::Green);
			}
			else
			{
				window->midimapFileProgress->setState(ProgressBarState::Red);
			}
		}
		break;
	case Message::EngineSettingsMessage:
		{
			EngineSettingsMessage *settings = (EngineSettingsMessage *)msg;
			window->lineedit->setText(settings->drumkitfile);
			if(settings->drumkit_loaded)
			{
				window->drumkitFileProgress->setProgress(1);
				window->drumkitFileProgress->setState(ProgressBarState::Green);
			}
			else
			{
				window->drumkitFileProgress->setProgress(0);
				window->drumkitFileProgress->setState(ProgressBarState::Blue);
			}
			window->lineedit2->setText(settings->midimapfile);
			if(settings->midimap_loaded)
			{
				window->midimapFileProgress->setProgress(1);
				window->midimapFileProgress->setState(ProgressBarState::Green);
			}
			else
			{
				window->midimapFileProgress->setProgress(0);
				window->midimapFileProgress->setState(ProgressBarState::Blue);
			}
			window->velocityCheck->setChecked(settings->enable_velocity_modifier);
			window->attackKnob->setValue(settings->velocity_modifier_weight);
			window->falloffKnob->setValue(settings->velocity_modifier_falloff);
		}
	default:
		break;
	}
}

bool PluginGUI::processEvents()
{
	if(!initialised)
	{
		return running;
	}

	window->eventHandler()->processEvents();
	//handleMessages();

	static bool foo = false;
	static int t = 0;
	if(t != time(nullptr))
	{
		t = time(nullptr);
		foo = !foo;
		float v = settings.velocity_modifier_falloff.load();
		v += 0.1f;
		settings.velocity_modifier_falloff.store(v);
	}

	Painter p(*window);

	// Run through all settings one at a time propagate changes to the UI.
	if(getter.enable_velocity_modifier.hasChanged())
	{
		enable_velocity_modifier_notifier(getter.enable_velocity_modifier.getValue());
	}

	if(getter.velocity_modifier_falloff.hasChanged())
	{
		velocity_modifier_falloff_notifier(getter.velocity_modifier_falloff.getValue());
	}

	if(getter.velocity_modifier_weight.hasChanged())
	{
		velocity_modifier_weight_notifier(getter.velocity_modifier_weight.getValue());
	}

	if(getter.enable_velocity_randomiser.hasChanged())
	{
		enable_velocity_randomiser_notifier(getter.enable_velocity_randomiser.getValue());
	}

	if(getter.velocity_randomiser_weight.hasChanged())
	{
		velocity_randomiser_weight_notifier(getter.velocity_randomiser_weight.getValue());
	}

	if(getter.samplerate.hasChanged())
	{
		samplerate_notifier(getter.samplerate.getValue());
	}

	if(getter.enable_resampling.hasChanged())
	{
		enable_resampling_notifier(getter.enable_resampling.getValue());
	}

	if(getter.number_of_files.hasChanged() ||
	   getter.number_of_files_loaded.hasChanged())
	{
		drumkit_file_progress_notifier((float)getter.number_of_files_loaded.getValue() /
		                               (float)getter.number_of_files.getValue());
	}

	//if(getter.current_file.hasChanged())
	//{
	//	current_file_notifier(getter.current_file.getValue());
	//}

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

	window = new DGWindow(native_window, msghandler, *config, settings);

	CONNECT(this, enable_velocity_modifier_notifier,
	        window->velocityCheck, &CheckBox::setChecked);

	CONNECT(this, velocity_modifier_falloff_notifier,
	        window->falloffKnob, &Knob::setValue);
	CONNECT(this, velocity_modifier_weight_notifier,
	        window->attackKnob, &Knob::setValue);


	//CONNECT(this, enable_velocity_randomiser_notifier,
	//        window->velocityCheck, &CheckBox::setChecked);
	//CONNECT(this, velocity_randomiser_weight_notifier,
	//        window->velocityCheck, &CheckBox::setChecked);

	//CONNECT(this, samplerate_notifier,
	//        window->velocityCheck, &CheckBox::setChecked);

	//CONNECT(this, enable_resampling_notifier,
	//        window->velocityCheck, &CheckBox::setChecked);

	CONNECT(this, drumkit_file_progress_notifier,
	        window->drumkitFileProgress, &ProgressBar::setProgress);

	auto eventHandler = window->eventHandler();
	CONNECT(eventHandler, closeNotifier, this, &PluginGUI::closeEventHandler);

	window->show();

	{ // Request all engine settings
		EngineSettingsMessage *msg = new EngineSettingsMessage();
		msghandler.sendMessage(MSGRCV_ENGINE, msg);
	}

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

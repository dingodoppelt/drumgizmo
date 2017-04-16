/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            mainwindow.h
 *
 *  Sat Nov 26 14:27:28 CET 2016
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
#pragma once

#include <settings.h>

#include "frame.h"
#include "image.h"
#include "textedit.h"
#include "tabwidget.h"
#include "texturedbox.h"
#include "window.h"
#include "maintab.h"

namespace GUI
{

class MainWindow
	: public Window
{
public:
	MainWindow(Settings& settings, void* native_window);

	//! Process all events and messages in queue
	//! \return true if not closing, returns false if closing.
	bool processEvents();

	//! Notify when window is closing.
	Notifier<> closeNotifier;
	void closeEventHandler();

private:
	void sizeChanged(std::size_t width, std::size_t height);

	// From Widget
	void repaintEvent(RepaintEvent* repaintEvent) override final;

	TabWidget tabs{this};

	MainTab main_tab{this};
	// TODO use some nice textfield widget here
	TextEdit gpl_text_field{&tabs};

	Image back{":bg.png"};
	Image logo{":logo.png"};

	TexturedBox sidebar{getImageCache(), ":sidebar.png",
			0, 0, // offset
			16, 0, 0, // delta-x
			14, 1, 14}; // delta-y

	TexturedBox topbar{getImageCache(), ":topbar.png",
			0, 0, // atlas offset (x, y)
			1, 1, 1, // dx1, dx2, dx3
			17, 1, 1}; // dy1, dy2, dy3

	bool closing{false};

	Settings& settings;
	SettingsNotifier settings_notifier{settings};

	Resource gpl{":../../COPYING"};
};

} // GUI::

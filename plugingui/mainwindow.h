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

#include <dggui/image.h>
#include <dggui/tabwidget.h>
#include <dggui/texturedbox.h>
#include <dggui/window.h>

#include "abouttab.h"
#include "drumkittab.h"
#include "maintab.h"
#include "pluginconfig.h"

namespace GUI
{

class MainWindow
	: public dggui::Window
{
public:

	static constexpr std::size_t main_width{750};
	static constexpr std::size_t main_height{740};

	MainWindow(Settings& settings, void* native_window);
	~MainWindow();

	//! Process all events and messages in queue
	//! \return true if not closing, returns false if closing.
	bool processEvents();

	//! Notify when window is closing.
	Notifier<> closeNotifier;
	void closeEventHandler();

private:
	void sizeChanged(std::size_t width, std::size_t height);
	void changeDrumkitTabVisibility(bool visible);

	// From Widget
	void repaintEvent(dggui::RepaintEvent* repaintEvent) override final;

	Config config;
	SettingsNotifier settings_notifier;

	dggui::TabWidget tabs{this};
	MainTab main_tab;
	DrumkitTab drumkit_tab;
	AboutTab about_tab{&tabs};

	dggui::Image back{":resources/bg.png"};

	dggui::TexturedBox sidebar{getImageCache(), ":resources/sidebar.png",
			0, 0, // offset
			16, 0, 0, // delta-x
			14, 1, 14}; // delta-y

	dggui::TexturedBox topbar{getImageCache(), ":resources/topbar.png",
			0, 0, // atlas offset (x, y)
			1, 1, 1, // dx1, dx2, dx3
			17, 1, 1}; // dy1, dy2, dy3

	bool closing{false};

	dggui::TabID drumkit_tab_id;
};

} // GUI::

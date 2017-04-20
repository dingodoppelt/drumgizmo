/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            tabwidget.h
 *
 *  Thu Nov 24 17:46:22 CET 2016
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

#include "widget.h"
#include "tabbutton.h"
#include "stackedwidget.h"
#include "texture.h"

namespace GUI
{

class TabWidget
	: public Widget
{
public:
	TabWidget(Widget *parent);

	//! Add new tab to the tab widget.
	//! \param title The title to display on the tab button.
	//! \param widget The widget to show in the tab.
	void addTab(const std::string& title, Widget* widget);

	std::size_t getBarHeight() const;

private:
	//! Callback for Widget::sizeChangeNotifier
	void sizeChanged(int width, int height);

private:
	void switchTab(Widget* tabWidget);
	void setActiveButtons(Widget* current_widget);

	std::list<TabButton> buttons;
	StackedWidget stack;

	TexturedBox topbar{getImageCache(), ":resources/topbar.png",
			0, 0, // atlas offset (x, y)
			1, 1, 1, // dx1, dx2, dx3
			17, 1, 1}; // dy1, dy2, dy3

	Texture toplogo{getImageCache(), ":resources/toplogo.png",
			0, 0, // atlas offset (x, y)
			95, 17}; // width, height
};

} // GUI::

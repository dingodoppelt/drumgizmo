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

namespace GUI
{

class TabWidget
	: public Widget
{
public:
	TabWidget(Widget *parent);

	void addTab(const std::string& title, Widget* widget);

private:
	//! Callback for Widget::sizeChangeNotifier
	void sizeChanged(int width, int height);

private:
	void switchTab(Widget* tabWidget);

	std::list<TabButton> buttons;
	StackedWidget stack;
};

} // GUI::

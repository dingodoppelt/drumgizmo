/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            stackedwidget.h
 *
 *  Mon Nov 21 19:36:49 CET 2016
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

#include <list>

#include "widget.h"
#include "notifier.h"

namespace dggui
{

//! A StackedWidget is a widget containing a list of widgets but only showing
//! one of them at a time.
//! It is be used to implement a TabWidget but can also be used for other
//! purposes.
class StackedWidget
	: public Widget
{
public:
	StackedWidget(Widget* parent);
	~StackedWidget();

	//! Add a widget to the stack.
	void addWidget(Widget* widget);

	//! Remove a widget from the stack.
	void removeWidget(Widget* widget);

	//! Get currently visible widget.
	Widget* getCurrentWidget() const;

	//! Show widget. Hide all the others.
	//! If widget is not in the stack nothing happens.
	void setCurrentWidget(Widget* widget);

	//! Returns a pointer to the Widget after the one referenced by 'widget' or
	//! nullptr if 'widget' is the last in the list.
	Widget* getWidgetAfter(Widget* widget);

	//! Returns a pointer to the Widget beforer the one referenced by 'widget' or
	//! nullptr if 'widget' is the first in the list.
	Widget* getWidgetBefore(Widget* widget);

	//! Reports whn a new widget is shown.
	Notifier<Widget*> currentChanged;

private:
	//! Callback for Widget::sizeChangeNotifier
	void sizeChanged(int width, int height);

private:
	Widget* currentWidget{nullptr};
	std::list<Widget*> widgets;
};

} // dggui::

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            stackedwidget.cc
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
#include "stackedwidget.h"

namespace GUI
{

StackedWidget::StackedWidget(Widget *parent)
	: Widget(parent)
{
	CONNECT(this, sizeChangeNotifier, this, &StackedWidget::sizeChanged);
}

StackedWidget::~StackedWidget()
{
}

void StackedWidget::addWidget(Widget *widget)
{
	widgets.push_back(widget);
	widget->reparent(this);

	if(currentWidget == nullptr)
	{
		setCurrentWidget(widget);
	}
	else
	{
		widget->setVisible(false);
	}
}

void StackedWidget::removeWidget(Widget *widget)
{
	if(widget == currentWidget)
	{
		setCurrentWidget(nullptr);
	}

	widgets.remove(widget);
}

Widget *StackedWidget::getCurrentWidget() const
{
	return currentWidget;
}

void StackedWidget::setCurrentWidget(Widget *widget)
{
	if(widget == currentWidget)
	{
		return;
	}

	if(currentWidget)
	{
		currentWidget->setVisible(false);
	}

	currentWidget = widget;

	if(currentWidget)
	{
		currentWidget->move(0, 0);
		currentWidget->resize(width(), height());
		currentWidget->setVisible(true);
	}

	currentChanged(currentWidget);
}

Widget* StackedWidget::getWidgetAfter(Widget* widget)
{
	bool found_it{false};

	for(auto w : widgets)
	{
		if(found_it)
		{
			return w;
		}

		if(w == widget)
		{
			found_it = true;
		}
	}

	if(found_it)
	{
		// widget was the last in the list.
		return nullptr;
	}

	// The Widget pointed to by 'widget' was not in the list...
	return nullptr;
}

Widget* StackedWidget::getWidgetBefore(Widget* widget)
{
	Widget* last{nullptr};

	for(auto w : widgets)
	{
		if(w == widget)
		{
			return last;
		}

		last = w;
	}

	// The Widget pointed to by 'widget' was not in the list...
	return nullptr;
}

void StackedWidget::sizeChanged(int width, int height)
{
	// Propagate size change to child:
	if(currentWidget)
	{
		currentWidget->move(0, 0);
		currentWidget->resize(width, height);
	}
}

} // GUI::

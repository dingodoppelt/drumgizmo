/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            tabwidget.cc
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
#include "tabwidget.h"

namespace GUI
{

TabWidget::TabWidget(Widget *parent)
	: Widget(parent)
	, stack(this)
{
	CONNECT(this, sizeChangeNotifier, this, &TabWidget::sizeChanged);
}

void TabWidget::addTab(const std::string& title, Widget* widget)
{
	buttons.emplace_back(this, widget);
	auto& button = buttons.back();
	button.setText(title);
	stack.addWidget(widget);
	CONNECT(&button, switchTabNotifier, this, &TabWidget::switchTab);
	sizeChanged(width(), height());
}

void TabWidget::switchTab(Widget* tabWidget)
{
	stack.setCurrentWidget(tabWidget);
}

void TabWidget::sizeChanged(int width, int height)
{
	std::size_t pos = 0;
	std::size_t buttonWidth = width / buttons.size();
	for(auto& button : buttons)
	{
		button.resize(buttonWidth, 40);
		button.move(pos, 0);
		pos += buttonWidth;
	}

	stack.move(0, 40);
	stack.resize(width, height - 40);
}

} // GUI::

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

#include "painter.h"

namespace GUI
{

TabWidget::TabWidget(Widget *parent)
	: Widget(parent)
	, stack(this)
{
	CONNECT(this, sizeChangeNotifier, this, &TabWidget::sizeChanged);
	CONNECT(&stack, currentChanged, this, &TabWidget::setActiveButtons);
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

std::size_t TabWidget::getBarHeight() const
{
	return topbar.height();
}

void TabWidget::switchTab(Widget* tabWidget)
{
	stack.setCurrentWidget(tabWidget);
}

void TabWidget::setActiveButtons(Widget* current_widget)
{
	for (auto& button : buttons) {
		if (button.getTabWidget() == current_widget) {
			button.setActive(true);
		}
		else {
			button.setActive(false);
		}
	}
}

void TabWidget::sizeChanged(int width, int height)
{
	std::size_t pos = 0;

	std::size_t button_width = 1;
	std::size_t bar_height = 25;
	std::size_t button_border_width = 10;

	std::size_t button_padding_left = 25;
	std::size_t button_padding_inner = 3;
	std::size_t logo_padding_right = button_padding_left / 2;

	Painter p(*this);
	p.clear();

	if(buttons.size() > 0)
	{
		for (auto& button : buttons)
		{
			auto min_width = button.getMinimalWidth();
			button_width = std::max(button_width, min_width + button_border_width);
		}

		button_width = std::min(button_width, width / buttons.size());
	}

	// draw the upper bar
	topbar.setSize(width, bar_height);
	p.drawImage(0, 0, topbar);
	auto x_logo = width - toplogo.width() - logo_padding_right;
	auto y_logo = (bar_height - toplogo.height()) / 2;
	p.drawImage(x_logo, y_logo, toplogo);

	// place the buttons
	pos = button_padding_left;
	for(auto& button : buttons)
	{
		button.resize(button_width, bar_height);
		button.move(pos, 0);
		pos += button_width + button_padding_inner;
	}

	stack.move(0, bar_height);
	stack.resize(width, height - bar_height);
}

} // GUI::

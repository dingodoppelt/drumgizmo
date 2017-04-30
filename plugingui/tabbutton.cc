/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            tabbutton.cc
 *
 *  Thu Nov 24 18:52:26 CET 2016
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
#include "tabbutton.h"

#include "painter.h"

namespace GUI
{

TabButton::TabButton(Widget* parent, Widget* tab_widget)
	: ButtonBase(parent)
	, tab_widget(tab_widget)
{
	CONNECT(this, clickNotifier, this, &TabButton::clickHandler);
}

TabButton::~TabButton()
{
}

Widget* TabButton::getTabWidget()
{
	return tab_widget;
}

std::size_t TabButton::getMinimalWidth() const
{
	std::size_t padding = 15;
	auto font_width = font.textWidth(text);

	return font_width + padding;
}

std::size_t TabButton::getMinimalHeight() const
{
	std::size_t padding = 10;
	auto font_height= font.textHeight(text);

	return font_height + padding;
}

void TabButton::setActive(bool active)
{
	this->active = active;

	if (active) {
		draw_state = State::Down;
	}
	else {
		draw_state = State::Up;
	}

	redraw();
}

void TabButton::repaintEvent(RepaintEvent* e)
{
	Painter p(*this);

	int padTop = 3;
	int padLeft = 0;
	int padTextTop = 3;

	int w = width();
	int h = height();
	if(w == 0 || h == 0)
	{
		return;
	}

	if (draw_state == State::Up && !active) {
		tab_passive.setSize(w - padLeft, h - padTop);
		p.drawImage(padLeft, padTop, tab_passive);
	}
	else {
		tab_active.setSize(w - padLeft, h - padTop);
		p.drawImage(padLeft, padTop, tab_active);
	}

	auto x = padLeft + (width() - font.textWidth(text)) / 2;
	auto y = padTop + padTextTop + font.textHeight(text);
	p.drawText(x, y, font, text, true);
}

void TabButton::scrollEvent(ScrollEvent* scroll_event)
{
	scrollNotifier(scroll_event->delta);
}

void TabButton::clickHandler()
{
	switchTabNotifier(tab_widget);
}

} // GUI::

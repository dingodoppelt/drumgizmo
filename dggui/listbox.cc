/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            listbox.cc
 *
 *  Mon Feb 25 21:21:41 CET 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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
#include "listbox.h"

#include "painter.h"
#include "font.h"

namespace dggui
{

ListBox::ListBox(Widget *parent)
	: Widget(parent)
	, selectionNotifier(basic.selectionNotifier)
	, clickNotifier(basic.clickNotifier)
	, valueChangedNotifier(basic.valueChangedNotifier)
	, basic(this)
{
	basic.move(7, 7);
}

ListBox::~ListBox()
{
}

void ListBox::addItem(std::string name, std::string value)
{
	basic.addItem(name, value);
}

void ListBox::addItems(std::vector<ListBoxBasic::Item> &items)
{
	basic.addItems(items);
}

void ListBox::clear()
{
	basic.clear();
}

bool ListBox::selectItem(int index)
{
	return basic.selectItem(index);
}

std::string ListBox::selectedName()
{
	return basic.selectedName();
}

std::string ListBox::selectedValue()
{
	return basic.selectedValue();
}

void ListBox::clearSelectedValue()
{
	basic.clearSelectedValue();
}

void ListBox::repaintEvent(RepaintEvent* repaintEvent)
{
	Painter p(*this);

	int w = width();
	int h = height();
	if(w == 0 || h == 0)
	{
		return;
	}

	box.setSize(w, h);
	p.drawImage(0, 0, box);
}

void ListBox::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);
	basic.resize(width - (7 + 7),
	             height - (7 + 7));
}

} // dggui::

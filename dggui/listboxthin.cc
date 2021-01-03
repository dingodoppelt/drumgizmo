/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            listboxthin.cc
 *
 *  Sun Apr  7 19:39:36 CEST 2013
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
#include "listboxthin.h"

#include "painter.h"
#include "font.h"

namespace dggui
{

ListBoxThin::ListBoxThin(Widget *parent)
	: Widget(parent)
	, selectionNotifier(basic.selectionNotifier)
	, clickNotifier(basic.clickNotifier)
	, valueChangedNotifier(basic.valueChangedNotifier)
	, basic(this)
{
	basic.move(1, 1);
}

ListBoxThin::~ListBoxThin()
{
}

void ListBoxThin::addItem(std::string name, std::string value)
{
	basic.addItem(name, value);
}

void ListBoxThin::addItems(std::vector<ListBoxBasic::Item> &items)
{
	basic.addItems(items);
}

void ListBoxThin::clear()
{
	basic.clear();
}

bool ListBoxThin::selectItem(int index)
{
	return basic.selectItem(index);
}

std::string ListBoxThin::selectedName()
{
	return basic.selectedName();
}

std::string ListBoxThin::selectedValue()
{
	return basic.selectedValue();
}

void ListBoxThin::repaintEvent(RepaintEvent* repaintEvent)
{
	Painter p(*this);

	int w = width();
	int h = height();
	if(w == 0 || h == 0)
	{
		return;
	}

	box.setSize(w,h);
	p.drawImage(0, 0, box);
}

void ListBoxThin::resize(std::size_t height, std::size_t width)
{
	Widget::resize(width, height);
	basic.resize(width - (1 + 1),
	             height - (1 + 1));
}

} // dggui::

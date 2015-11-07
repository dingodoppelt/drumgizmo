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
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "listbox.h"

#include "painter.h"
#include "font.h"

namespace GUI {

ListBox::ListBox(Widget *parent)
	: Widget(parent)
	, selectionNotifier(basic.selectionNotifier)
	, clickNotifier(basic.clickNotifier)
	, valueChangedNotifier(basic.valueChangedNotifier)
	, basic(this)
{
	box.topLeft     = new Image(":widget_tl.png");
	box.top         = new Image(":widget_t.png");
	box.topRight    = new Image(":widget_tr.png");
	box.left        = new Image(":widget_l.png");
	box.right       = new Image(":widget_r.png");
	box.bottomLeft  = new Image(":widget_bl.png");
	box.bottom      = new Image(":widget_b.png");
	box.bottomRight = new Image(":widget_br.png");
	box.center      = new Image(":widget_c.png");

	basic.move(box.left->width(), box.top->height());
}

ListBox::~ListBox()
{
	delete box.topLeft;
	delete box.top;
	delete box.topRight;
	delete box.left;
	delete box.right;
	delete box.bottomLeft;
	delete box.bottom;
	delete box.bottomRight;
	delete box.center;
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

void ListBox::repaintEvent(RepaintEvent *e)
{
	Painter p(*this);

	p.clear();

	int w = width();
	int h = height();
	if(w == 0 || h == 0)
	{
		return;
	}

	p.drawBox(0, 0, box, w, h);
}

void ListBox::resize(int width, int height)
{
	Widget::resize(width, height);
	basic.resize(width - (box.left->width() + box.right->width()),
	             height - (box.top->height() + box.bottom->height()));
}

} // GUI::

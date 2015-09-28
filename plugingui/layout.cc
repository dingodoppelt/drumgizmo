/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            layout.cc
 *
 *  Sat Mar 21 15:12:36 CET 2015
 *  Copyright 2015 Bent Bisballe Nyeng
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
#include "layout.h"

#include "widget.h"

namespace GUI {

LayoutItem::LayoutItem()
	: parent(nullptr)
{
}

LayoutItem::~LayoutItem()
{
	setLayoutParent(nullptr); // Will disconnect from layout if any.
}

void LayoutItem::setLayoutParent(Layout *p)
{
	if(this->parent)
	{
		this->parent->removeItem(this);
	}

	this->parent = p;
}

Layout::Layout(LayoutItem *parent)
{
	this->parent = parent;
}

void Layout::addItem(LayoutItem *item)
{
	items.push_back(item);
	item->setLayoutParent(this);
	layout();
}

void Layout::removeItem(LayoutItem *item)
{
	LayoutItemList::iterator i = items.begin();
	while(i != items.end())
	{
		if(*i == item)
		{
			i = items.erase(i);
			continue;
		}
		++i;
	}

	layout();
}

//
// BoxLayout
//

BoxLayout::BoxLayout(LayoutItem *parent)
	: Layout(parent)
{
	resize_children = true;
}

void BoxLayout::setResizeChildren(bool resize_children)
{
	this->resize_children = resize_children;
}

//
// VBoxLayout
//

VBoxLayout::VBoxLayout(LayoutItem *parent)
	: BoxLayout(parent), align(HALIGN_CENTER)
{
}

void VBoxLayout::layout()
{
	size_t y = 0;
	size_t w = parent->width();
	size_t h = parent->height() / items.size();

	LayoutItemList::iterator i = items.begin();
	while(i != items.end())
	{
		LayoutItem *item = *i;
		if(resize_children)item->resize(w, h);
		item->move(0, y);
		y += h;
		++i;
	}
}

//
// HBoxLayout
//

HBoxLayout::HBoxLayout(LayoutItem *parent)
	: BoxLayout(parent)
	, align(VALIGN_CENTER)
{
}

void HBoxLayout::layout()
{
	size_t w = parent->width() / items.size();
	size_t h = parent->height();
	size_t x = 0;

	LayoutItemList::iterator i = items.begin();
	while(i != items.end())
	{
		LayoutItem *item = *i;
		if(resize_children)
		{
			item->resize(w, h);
			item->move(x, 0);
		}
		else
		{
			size_t y = 0;
			switch(align) {
			case VALIGN_TOP:
				y = 0;
				break;
			case VALIGN_CENTER:
				y = (h / 2) - (item->height() / 2);
				break;
			case VALIGN_BOTTOM:
				y = h - item->height();
				break;
			}

			int diff = w - item->width();
			item->move(x + diff / 2, y);
		}
		x += w;
		++i;
	}
}

void HBoxLayout::setVAlignment(alignment_t alignment)
{
	align = alignment;
}

} // GUI::

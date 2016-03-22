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
	: parent(parent)
{
	auto widget = dynamic_cast<Widget*>(parent);
	if(widget)
	{
		CONNECT(widget, sizeChangeNotifier, this, &Layout::sizeChanged);
	}
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

void Layout::sizeChanged(int width, int height)
{
	layout();
}

//
// BoxLayout
//

BoxLayout::BoxLayout(LayoutItem *parent)
	: Layout(parent)
{
}

void BoxLayout::setResizeChildren(bool resizeChildren)
{
	this->resizeChildren = resizeChildren;
	layout();
}

void BoxLayout::setSpacing(size_t spacing)
{
	this->spacing = spacing;
	layout();
}

//
// VBoxLayout
//

VBoxLayout::VBoxLayout(LayoutItem *parent)
	: BoxLayout(parent)
	, align(HAlignment::center)
{
}

void VBoxLayout::layout()
{
	size_t y = 0;
	size_t w = parent->width();
	//size_t h = parent->height() / items.size();

	LayoutItemList::iterator i = items.begin();
	while(i != items.end())
	{
		LayoutItem *item = *i;
		if(resizeChildren)
		{
			item->resize(w, parent->height() / items.size());
		}

		size_t x = 0;
		switch(align) {
		case HAlignment::left:
			x = 0;
			break;
		case HAlignment::center:
			x = (w / 2) - (item->width() / 2);
			break;
		case HAlignment::right:
			x = w - item->width();
			break;
		}

		item->move(x, y);
		y += item->height() + spacing;
		++i;
	}
}

void VBoxLayout::setHAlignment(HAlignment alignment)
{
	align = alignment;
}

//
// HBoxLayout
//

HBoxLayout::HBoxLayout(LayoutItem *parent)
	: BoxLayout(parent)
	, align(VAlignment::center)
{
}

void HBoxLayout::layout()
{
	if(items.empty())
	{
		return;
	}

//	size_t w = parent->width() / items.size();
	size_t h = parent->height();
	size_t x = 0;

	LayoutItemList::iterator i = items.begin();
	while(i != items.end())
	{
		LayoutItem *item = *i;
		if(resizeChildren)
		{
			item->resize(parent->width() / items.size(), h);
			item->move(x, 0);
		}
		else
		{
			size_t y = 0;
			switch(align) {
			case VAlignment::top:
				y = 0;
				break;
			case VAlignment::center:
				y = (h / 2) - (item->height() / 2);
				break;
			case VAlignment::bottom:
				y = h - item->height();
				break;
			}

			int diff = 0;//w - item->width();
			item->move(x + diff / 2, y);
		}
		x += item->width() + spacing;
		++i;
	}
}

void HBoxLayout::setVAlignment(VAlignment alignment)
{
	align = alignment;
}

} // GUI::

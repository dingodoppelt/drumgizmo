/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            listboxbasic.cc
 *
 *  Thu Apr  4 20:28:10 CEST 2013
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
#include "listboxbasic.h"

#include "painter.h"
#include "font.h"

namespace GUI {

void scrolled(void *ptr)
{
	ListBoxBasic *l = (ListBoxBasic *)ptr;
	l->repaintEvent(NULL);
}

ListBoxBasic::ListBoxBasic(Widget *parent)
	: Widget(parent)
	, scroll(this)
	, bg_img(":widget_c.png")
{
	scroll.move(0,0);
	scroll.resize(18, 100);

	scroll.registerValueChangeHandler(scrolled, this);

	padding = 4;
	btn_size = 18;

	selected = -1;
	marked = -1;
}

ListBoxBasic::~ListBoxBasic()
{
}

void ListBoxBasic::setSelection(int index)
{
	selected = index;
	valueChangedNotifier();
}

void ListBoxBasic::addItem(std::string name, std::string value)
{
	std::vector<ListBoxBasic::Item> items;
	ListBoxBasic::Item item;
	item.name = name;
	item.value = value;
	items.push_back(item);
	addItems(items);
}

void ListBoxBasic::addItems(std::vector<ListBoxBasic::Item> &newItems)
{
	for(auto& item : newItems)
	{
		items.push_back(item);
	}

	if(selected == -1)
	{
		setSelection((int)items.size() - 1);
	}

	setSelection(0);

	int numitems = height() / (font.textHeight() + padding);
	scroll.setRange(numitems);
	scroll.setMaximum(items.size());
	repaintEvent(nullptr);
}

void ListBoxBasic::clear()
{
	items.clear();
	setSelection(-1);
	scroll.setValue(0);
	repaintEvent(nullptr);
}

bool ListBoxBasic::selectItem(int index)
{
	if(index < 0 || (index > (int)items.size() - 1))
	{
		return false;
	}

	setSelection(index);
	repaintEvent(nullptr);

	return true;
}

std::string ListBoxBasic::selectedName()
{
	if(selected < 0 || (selected > (int)items.size() - 1))
	{
		return "";
	}

	return items[selected].name;
}

std::string ListBoxBasic::selectedValue()
{
	if(selected < 0 || (selected > (int)items.size() - 1))
	{
		return "";
	}

	return items[selected].value;
}

void ListBoxBasic::clearSelectedValue()
{
	setSelection(-1);
}

void ListBoxBasic::repaintEvent(RepaintEvent *e)
{
	Painter p(this);

	p.clear();

	int w = width();
	int h = height();

	if(w == 0 || h == 0)
	{
		return;
	}

	p.drawImageStretched(0, 0, &bg_img, w, h);

	p.setColour(Colour(183.0/255.0, 219.0/255.0 , 255.0/255.0, 1));

	int yoffset = padding / 2;
	int skip = scroll.value();
	int numitems = height() / (font.textHeight() + padding) + 1;
	for(int idx = skip; (idx < (int)items.size()) && (idx < (skip + numitems));
	    idx++)
	{
		auto& item = items[idx];
		if(idx == selected)
		{
			p.setColour(Colour(183.0/255.0, 219.0/255.0 , 255.0/255.0, 0.5));
			p.drawFilledRectangle(0,
			                      yoffset - (padding / 2),
			                      width() - 1,
			                      yoffset + (font.textHeight() + 1));
		}

		if(idx == marked)
		{
			p.drawRectangle(0,
			                yoffset - (padding / 2),
			                width() - 1,
			                yoffset + (font.textHeight() + 1));
		}

		p.setColour(Colour(183.0/255.0, 219.0/255.0 , 255.0/255.0, 1));

		p.drawText(2, yoffset + font.textHeight(), font, item.name);
		yoffset += font.textHeight() + padding;
	}
}

void ListBoxBasic::scrollEvent(ScrollEvent *e)
{
	scroll.scrollEvent(e);
}

void ListBoxBasic::keyEvent(KeyEvent *e)
{
	if(e->direction != KeyEvent::Up)
	{
		return;
	}

	switch(e->keycode) {
	case KeyEvent::KeyUp:
		if(marked == 0)
		{
			return;
		}

		marked--;

		if(marked < scroll.value())
		{
			scroll.setValue(marked);
		}
		break;

	case KeyEvent::KeyDown:
		{
			// Number of items that can be displayed at a time.
			int numitems = height() / (font.textHeight() + padding);

			if(marked == ((int)items.size() - 1))
			{
				return;
			}

			marked++;

			if(marked > (scroll.value() + numitems - 1))
			{
				scroll.setValue(marked - numitems + 1);
			}
		}
		break;

	case KeyEvent::KeyHome:
		marked = 0;
		if(marked < scroll.value())
		{
			scroll.setValue(marked);
		}
		break;

	case KeyEvent::KeyEnd:
		{
			// Number of items that can be displayed at a time.
			int numitems = height() / (font.textHeight() + padding);

			marked = (int)items.size() - 1;
			if(marked > (scroll.value() + numitems - 1))
			{
				scroll.setValue(marked - numitems + 1);
			}
		}
		break;

	case KeyEvent::KeyCharacter:
		if(e->text == " ")
		{
			setSelection(marked);
			//selectionNotifier();
		}
		break;

	case KeyEvent::KeyEnter:
		setSelection(marked);
		selectionNotifier();
		break;

	default:
		break;
	}

	repaintEvent(nullptr);
}

void ListBoxBasic::buttonEvent(ButtonEvent *e)
{
	if((e->x > ((int)width() - btn_size)) && (e->y < ((int)width() - 1)))
	{
		if(e->y > 0 && e->y < btn_size)
		{
			if(e->direction == ButtonEvent::Up)
			{
				return;
			}
			scroll.setValue(scroll.value() - 1);
			return;
		}

		if(e->y > ((int)height() - btn_size) && e->y < ((int)height() - 1))
		{
			if(e->direction == ButtonEvent::Up)
			{
				return;
			}
			scroll.setValue(scroll.value() + 1);
			return;
		}
	}

	if(e->direction == ButtonEvent::Up)
	{
		int skip = scroll.value();
		size_t yoffset = padding / 2;
		for(int idx = skip; idx < (int)items.size(); idx++)
		{
			yoffset += font.textHeight() + padding;
			if(e->y < (int)yoffset - (padding / 2))
			{
				setSelection(idx);
				marked = selected;
				clickNotifier();
				break;
			}
		}

		repaintEvent(nullptr);
	}

	if(e->direction != ButtonEvent::Up)
	{
		int skip = scroll.value();
		size_t yoffset = padding / 2;
		for(int idx = skip; idx < (int)items.size(); idx++)
		{
			yoffset += font.textHeight() + padding;
			if(e->y < ((int)yoffset - (padding / 2)))
			{
				marked = idx;
				break;
			}
		}

		repaintEvent(nullptr);
	}

	if(e->doubleclick)
	{
		selectionNotifier();
	}
}

void ListBoxBasic::resize(int w, int h)
{
	Widget::resize(w,h);
	scroll.move(w - scroll.width(), 0);
	scroll.resize(scroll.width(), h);
}

} // GUI::

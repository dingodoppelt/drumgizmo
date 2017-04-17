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
#include "listboxbasic.h"

#include "painter.h"
#include "font.h"

namespace GUI {

ListBoxBasic::ListBoxBasic(Widget *parent)
	: Widget(parent)
	, scroll(this)
{
	scroll.move(0,0);
	scroll.resize(16, 100);

	CONNECT(&scroll, valueChangeNotifier,
	        this, &ListBoxBasic::onScrollBarValueChange);

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
	if(marked == -1)
	{
		marked = index;
	}
	valueChangedNotifier();
}

void ListBoxBasic::addItem(const std::string& name, const std::string& value)
{
	std::vector<ListBoxBasic::Item> items;
	ListBoxBasic::Item item;
	item.name = name;
	item.value = value;
	items.push_back(item);
	addItems(items);
}

void ListBoxBasic::addItems(const std::vector<ListBoxBasic::Item>& newItems)
{
	for(auto& item : newItems)
	{
		items.push_back(item);
	}

	if(selected == -1)
	{
		//setSelection((int)items.size() - 1);
		setSelection(0);
	}
	redraw();
}

void ListBoxBasic::clear()
{
	items.clear();
	setSelection(-1);
	marked = -1;
	scroll.setValue(0);
	redraw();
}

bool ListBoxBasic::selectItem(int index)
{
	if(index < 0 || (index > (int)items.size() - 1))
	{
		return false;
	}

	setSelection(index);
	redraw();

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

void ListBoxBasic::onScrollBarValueChange(int value)
{
	redraw();
}

void ListBoxBasic::repaintEvent(RepaintEvent* repaintEvent)
{
	Painter p(*this);
	p.clear();

	int w = width();
	int h = height();

	if((w == 0) || (h == 0))
	{
		return;
	}

	p.drawImageStretched(0, 0, bg_img, w, h);

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

	scroll.setRange(numitems);
	scroll.setMaximum(items.size());
}

void ListBoxBasic::scrollEvent(ScrollEvent* scrollEvent)
{
	// forward scroll event to scroll bar.
	scroll.scrollEvent(scrollEvent);
}

void ListBoxBasic::keyEvent(KeyEvent* keyEvent)
{
	if(keyEvent->direction != Direction::down)
	{
		return;
	}

	switch(keyEvent->keycode) {
	case Key::up:
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

	case Key::down:
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

	case Key::home:
		marked = 0;
		if(marked < scroll.value())
		{
			scroll.setValue(marked);
		}
		break;

	case Key::end:
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

	case Key::character:
		if(keyEvent->text == " ")
		{
			setSelection(marked);
			//selectionNotifier();
		}
		break;

	case Key::enter:
		setSelection(marked);
		selectionNotifier();
		break;

	default:
		break;
	}

	redraw();
}

void ListBoxBasic::buttonEvent(ButtonEvent* buttonEvent)
{
	// Ignore everything except left clicks.
	if(buttonEvent->button != MouseButton::left)
	{
		return;
	}

	if((buttonEvent->x > ((int)width() - btn_size)) &&
	   (buttonEvent->y < ((int)width() - 1)))
	{
		if(buttonEvent->y > 0 && buttonEvent->y < btn_size)
		{
			if(buttonEvent->direction == Direction::up)
			{
				return;
			}
			scroll.setValue(scroll.value() - 1);
			return;
		}

		if(buttonEvent->y > ((int)height() - btn_size) &&
		   buttonEvent->y < ((int)height() - 1))
		{
			if(buttonEvent->direction == Direction::up)
			{
				return;
			}
			scroll.setValue(scroll.value() + 1);
			return;
		}
	}

	if(buttonEvent->direction == Direction::up)
	{
		int skip = scroll.value();
		size_t yoffset = padding / 2;
		for(int idx = skip; idx < (int)items.size(); idx++)
		{
			yoffset += font.textHeight() + padding;
			if(buttonEvent->y < (int)yoffset - (padding / 2))
			{
				setSelection(idx);
				marked = selected;
				clickNotifier();
				break;
			}
		}

		redraw();
	}

	if(buttonEvent->direction != Direction::up)
	{
		int skip = scroll.value();
		size_t yoffset = padding / 2;
		for(int idx = skip; idx < (int)items.size(); idx++)
		{
			yoffset += font.textHeight() + padding;
			if(buttonEvent->y < ((int)yoffset - (padding / 2)))
			{
				marked = idx;
				break;
			}
		}

		redraw();
	}

	if(buttonEvent->doubleClick)
	{
		selectionNotifier();
	}
}

void ListBoxBasic::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);
	scroll.move(width - scroll.width(), 0);
	scroll.resize(scroll.width(), height);
}

} // GUI::

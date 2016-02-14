/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            widget.cc
 *
 *  Sun Oct  9 13:01:44 CEST 2011
 *  Copyright 2011 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
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
#include "widget.h"

#include "painter.h"
#include "window.h"

#include <stdio.h>

namespace GUI {

Widget::Widget(Widget* parent)
	: parent(parent)
{
	if(parent)
	{
		parent->addChild(this);
		_window = parent->window();
	}

	_width = _height = 0;
	_visible = true;
}

Widget::~Widget()
{
	if(parent)
	{
		parent->removeChild(this);
	}
}

void Widget::show()
{
	setVisible(true);
}

void Widget::hide()
{
	setVisible(false);
}

void Widget::setVisible(bool visible)
{
	_visible = visible;
	repaintEvent(nullptr);
}

bool Widget::visible()
{
	return _visible;
}

void Widget::addChild(Widget* widget)
{
	children.push_back(widget);
}

void Widget::removeChild(Widget* widget)
{
	for(auto i = children.begin(); i != children.end(); ++i)
	{
		if(*i == widget)
		{
			children.erase(i);
			return;
		}
	}
}

void Widget::resize(int width, int height)
{
	if((width < 1) || (height < 1) ||
	   (((size_t)width == _width) && ((size_t)height == _height)))
	{
		return;
	}

	_width = width;
	_height = height;
	pixbuf.realloc(width, height);

	sizeChangeNotifier(width, height);
}

void Widget::move(size_t x, size_t y)
{
	_x = x;
	_y = y;
}

int Widget::x()
{
	return _x;
}

int Widget::y()
{
	return _y;
}

size_t Widget::width()
{
	return _width;
}

size_t Widget::height()
{
	return _height;
}

size_t Widget::windowX()
{
	size_t window_x = x();
	if(parent)
	{
		window_x += parent->windowX();
	}

	return window_x;
}

size_t Widget::windowY()
{
	size_t window_y = y();
	if(parent)
	{
		window_y += parent->windowY();
	}

	return window_y;
}

Widget* Widget::find(int x, int y)
{
	for(auto i = children.rbegin(); i != children.rend(); ++i)
	{
		Widget* widget = *i;
		if(widget->visible())
		{
			if((x >= widget->x()) && (x < (widget->x() + (int)widget->width())) &&
			   (y >= widget->y()) && (y < (widget->y() + (int)widget->height())))
			{
				return widget->find(x - widget->x(), y - widget->y());
			}
		}
	}

	return this;
}

Window* Widget::window()
{
	return _window;
}

std::vector<PixelBufferAlpha*> Widget::getPixelBuffers()
{
	std::vector<PixelBufferAlpha*> pixelBuffers;

	pixbuf.x = windowX();
	pixbuf.y = windowY();

	pixelBuffers.push_back(&pixbuf);

	for(auto child : children)
	{
		if(child->visible())
		{
			auto childPixelBuffers = child->getPixelBuffers();
			pixelBuffers.insert(pixelBuffers.end(),
			                    childPixelBuffers.begin(), childPixelBuffers.end());
		}
	}

	return pixelBuffers;
}

bool Widget::hasKeyboardFocus()
{
	return window()->keyboardFocus() == this;
}

void Widget::repaintChildren(RepaintEvent* repaintEvent)
{
	Painter p(*this); // make sure pixbuf refcount is incremented.

	this->repaintEvent(repaintEvent);

	for(auto child : children)
	{
		child->repaintChildren(repaintEvent);
	}
}

} // GUI::

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
#include "widget.h"

#include "painter.h"
#include "window.h"

#include <stdio.h>

namespace GUI {

Widget::Widget(Widget *parent)
	: pixbuf(1, 1)
{
	_width = _height = 10;

	this->parent = parent;
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

void Widget::setVisible(bool v)
{
	_visible = v;
	repaintEvent(nullptr);
}

bool Widget::visible()
{
	return _visible;
}

void Widget::addChild(Widget *widget)
{
	children.push_back(widget);
}

void Widget::removeChild(Widget *widget)
{
	std::vector<Widget *>::iterator i = children.begin();
	while(i != children.end())
	{
		if(*i == widget)
		{
			children.erase(i);
			return;
		}
		i++;
	}
}

void Widget::resize(int width, int height)
{
	if(width < 1 || height < 1)
	{
		return;
	}

	_width = width;
	_height = height;
	pixbuf.realloc(width, height);
}

void Widget::move(size_t x, size_t y)
{
	_x = x;
	_y = y;
}

size_t Widget::x() { return _x; }
size_t Widget::y() { return _y; }
size_t Widget::width() { return _width; }
size_t Widget::height() { return _height; }

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

Widget *Widget::find(size_t x, size_t y)
{
	std::vector<Widget*>::reverse_iterator i = children.rbegin();
	while(i != children.rend())
	{
		Widget *w = *i;
		if(w->visible())
		{
			if(w->x() <= x && (w->x() + w->width()) >= x &&
			   w->y() <= y && w->y() + w->height() >= y)
			{
				return w->find(x - w->x(), y - w->y());
			}
		}
		i++;
	}

	if(x > width() || x < 0 || y > height() || y < 0)
	{
		return nullptr;
	}

	return this;
}

Window *Widget::window()
{
	return _window;
}

void Widget::repaint_r(RepaintEvent *e)
{
	Painter p(this); // make sure pixbuf refcount is incremented.

	repaintEvent(e);

	std::vector<Widget*>::iterator i = children.begin();
	while(i != children.end())
	{
		Widget *w = *i;
		w->repaint_r(e);
		i++;
	}
}

std::vector<PixelBufferAlpha *> Widget::getPixelBuffers()
{
	std::vector<PixelBufferAlpha *> pbs;

	pixbuf.x = windowX();
	pixbuf.y = windowY();

	pbs.push_back(&pixbuf);

	std::vector<Widget*>::iterator i = children.begin();
	while(i != children.end())
	{
		Widget *w = *i;
		if(w->visible())
		{
			std::vector<PixelBufferAlpha *> pbs0 = w->getPixelBuffers();
			pbs.insert(pbs.end(), pbs0.begin(), pbs0.end());
		}
		i++;
	}

	return pbs;
}

bool Widget::hasKeyboardFocus()
{
	return window()->keyboardFocus() == this;
}

} // GUI::

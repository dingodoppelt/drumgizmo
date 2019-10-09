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
#include "widget.h"

#include <cassert>

#include "painter.h"
#include "window.h"

namespace GUI
{

Widget::Widget(Widget* parent)
	: parent(parent)
{
	if(parent)
	{
		parent->addChild(this);
		_window = parent->window();
	}

	pixbuf.x = translateToWindowX();
	pixbuf.y = translateToWindowY();
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
	pixbuf.visible = visible;
	redraw();
}

bool Widget::visible() const
{
	return _visible;
}

void Widget::redraw()
{
	dirty = true;
	window()->needsRedraw();
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

void Widget::reparent(Widget* parent)
{
	if(parent == this->parent)
	{
		return; // Already at the right parent.
	}

	if(this->parent)
	{
		this->parent->removeChild(this);
	}

	if(parent)
	{
		parent->addChild(this);
	}

	this->parent = parent;
}

void Widget::resize(std::size_t width, std::size_t height)
{
	assert(width < 32000 && height < 32000); // Catch negative values as size_t
	if((width < 1) || (height < 1) ||
	   ((width == _width) && (height == _height)))
	{
		return;
	}

	_width = width;
	_height = height;

	// Store old size/position in pixelbuffer for rendering invalidation.
	if(!pixbuf.has_last)
	{
		pixbuf.last_width = pixbuf.width;
		pixbuf.last_height = pixbuf.height;
		pixbuf.last_x = pixbuf.x;
		pixbuf.last_y = pixbuf.y;
		pixbuf.has_last = true;
	}

	pixbuf.realloc(width, height);
	pixbuf.x = translateToWindowX();
	pixbuf.y = translateToWindowY();
	redraw();
	sizeChangeNotifier(width, height);
}

void Widget::move(int x, int y)
{
	if((_x == x) &&
	   (_y == y))
	{
		return;
	}

	_x = x;
	_y = y;

	// Store old size/position in pixelbuffer for rendering invalidation.
	if(!pixbuf.has_last)
	{
		pixbuf.last_width = pixbuf.width;
		pixbuf.last_height = pixbuf.height;
		pixbuf.last_x = pixbuf.x;
		pixbuf.last_y = pixbuf.y;
		pixbuf.has_last = true;
	}

	//pixbuf.x = translateToWindowX();
	//pixbuf.y = translateToWindowY();

	positionChangeNotifier(x, y);
}

int Widget::x() const
{
	return _x;
}

int Widget::y() const
{
	return _y;
}

std::size_t Widget::width() const
{
	return _width;
}

std::size_t Widget::height() const
{
	return _height;
}

Point Widget::position() const
{
	return { _x, _y };
}

PixelBufferAlpha& Widget::GetPixelBuffer()
{
	return pixbuf;
}

ImageCache& Widget::getImageCache()
{
	assert(parent);
	return parent->getImageCache();
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

	pixbuf.x = translateToWindowX();
	pixbuf.y = translateToWindowY();

	if(dirty)
	{
		repaintEvent(nullptr);
		pixbuf.dirty = true;
		dirty = false;
	}

	if(pixbuf.dirty || visible())
	{
		pixelBuffers.push_back(&pixbuf);
	}

	if(visible())
	{
		for(auto child : children)
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

std::size_t Widget::translateToWindowX()
{
	size_t window_x = x();
	if(parent)
	{
		window_x += parent->translateToWindowX();
	}

	return window_x;
}

std::size_t Widget::translateToWindowY()
{
	size_t window_y = y();
	if(parent)
	{
		window_y += parent->translateToWindowY();
	}

	return window_y;
}

} // GUI::

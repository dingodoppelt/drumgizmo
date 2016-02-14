/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            window.cc
 *
 *  Sun Oct  9 13:11:53 CEST 2011
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
#include "window.h"

#include <hugin.hpp>
#include "painter.h"

#ifndef PUGL
#ifdef X11
#include "nativewindow_x11.h"
#endif/*X11*/
#ifdef WIN32
#include "nativewindow_win32.h"
#endif/*WIN32*/
#else
#include "nativewindow_pugl.h"
#endif

namespace GUI {

Window::Window()
	: Widget(nullptr)
	, wpixbuf(100, 100)
{
	// Make sure we have a valid size when initialising the NativeWindow
	_width = wpixbuf.width;
	_height = wpixbuf.height;

#ifndef PUGL
#ifdef X11
	native = new NativeWindowX11(*this);
#endif/*X11*/
#ifdef WIN32
	native = new NativeWindowWin32(*this);
#endif/*WIN32*/
#else/*Use pugl*/
	native = new NativeWindowPugl(this);
#endif

	eventhandler = new EventHandler(*native, *this);
}

Window::~Window()
{
	delete native;
	delete eventhandler;
}

void Window::setFixedSize(int w, int h)
{
	native->setFixedSize(w, h);
	resize(w,h);
}

void Window::setCaption(const std::string& caption)
{
	native->setCaption(caption);
}

void Window::resize(int width, int height)
{
	if((width < 1) || (height < 1))
	{
		return;
	}

	resized(width, height);
	Widget::resize(width, height);
	native->resize(width, height);
}

void Window::move(size_t x, size_t y)
{
	native->move(x, y);

	// Make sure widget corrdinates are updated.
	Widget::move(x, y);
}

size_t Window::windowX()
{
	return 0;
}

size_t Window::windowY()
{
	return 0;
}

void Window::show()
{
	repaintChildren(nullptr);
	native->show();
}

void Window::hide()
{
	native->hide();
}

Window* Window::window()
{
	return this;
}

EventHandler* Window::eventHandler()
{
	return eventhandler;
}

Widget* Window::keyboardFocus()
{
	return _keyboardFocus;
}

void Window::setKeyboardFocus(Widget* widget)
{
	auto oldFocusWidget = _keyboardFocus;
	_keyboardFocus = widget;

	if(oldFocusWidget)
	{
		oldFocusWidget->repaintEvent(nullptr);
	}

	if(_keyboardFocus)
	{
		_keyboardFocus->repaintEvent(nullptr);
	}
}

Widget* Window::buttonDownFocus()
{
	return _buttonDownFocus;
}

void Window::setButtonDownFocus(Widget* widget)
{
	_buttonDownFocus = widget;
	native->grabMouse(widget != nullptr);
}

Widget* Window::mouseFocus()
{
	return _mouseFocus;
}

void Window::setMouseFocus(Widget* widget)
{
	_mouseFocus = widget;

}

void Window::redraw()
{
	native->redraw();
}

void Window::resized(size_t width, size_t height)
{
	if((_width == width) && (_height == height))
	{
		return;
	}

	_width = width;
	_height = height;

	wpixbuf.realloc(width, height);
	updateBuffer();

	pixbuf.realloc(width, height);
	repaintEvent(nullptr);

	// Notify Widget
	sizeChangeNotifier(width, height);
}

void Window::updateBuffer()
{
	//DEBUG(window, "Updating buffer\n");
	for(auto pixelBuffer : getPixelBuffers())
	{
		size_t updateWidth = pixelBuffer->width;
		size_t updateHeight = pixelBuffer->height;

		// Skip buffer if not inside window.
		if((wpixbuf.width < pixelBuffer->x) || (wpixbuf.height < pixelBuffer->y))
		{
			continue;
		}

		if(updateWidth > (wpixbuf.width - pixelBuffer->x))
		{
			updateWidth = (wpixbuf.width - pixelBuffer->x);
		}

		if(updateHeight > (wpixbuf.height - pixelBuffer->y))
		{
			updateHeight = (wpixbuf.height - pixelBuffer->y);
		}

		unsigned char r,g,b,a;
		for(size_t y = 0; y < updateHeight; y++)
		{
			for(size_t x = 0; x < updateWidth; x++)
			{
				pixelBuffer->pixel(x, y, &r, &g, &b, &a);
				wpixbuf.setPixel(x + pixelBuffer->x, y + pixelBuffer->y, r, g, b, a);
			}
		}
	}

	native->handleBuffer();
}

void Window::beginPaint()
{
	++refcount;
	if(refcount > maxRefcount)
	{
		maxRefcount = refcount;
	}
}

void Window::endPaint()
{
	if(refcount)
	{
		--refcount;
	}

	if(!refcount)
	{
		// Did we go deep enough for a buffer update?
		if(maxRefcount > 1)
		{
			updateBuffer();
			redraw();
		}
		maxRefcount = 0;
	}
}

} // GUI::

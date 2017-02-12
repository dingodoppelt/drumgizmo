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
#include "window.h"

#include "painter.h"

#ifndef PUGL
#ifdef X11
#include "nativewindow_x11.h"
#endif/*X11*/
#ifdef WIN32
#include "nativewindow_win32.h"
#endif/*WIN32*/
#ifdef COCOA
#include "nativewindow_cocoa.h"
#endif/*COCOA*/
#else
#include "nativewindow_pugl.h"
#endif

namespace GUI
{

Window::Window(void* native_window)
	: Widget(nullptr)
	, wpixbuf(1, 1)
{
	// Make sure we have a valid size when initialising the NativeWindow
	_width = wpixbuf.width;
	_height = wpixbuf.height;

#ifndef PUGL
#ifdef X11
	native = new NativeWindowX11(native_window, *this);
#endif/*X11*/
#ifdef WIN32
	native = new NativeWindowWin32(native_window, *this);
#endif/*WIN32*/
#ifdef COCOA
	native = new NativeWindowCocoa(native_window, *this);
#endif/*COCOA*/
#else/*Use pugl*/
	native = new NativeWindowPugl(native_window, *this);
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
}

void Window::setCaption(const std::string& caption)
{
	native->setCaption(caption);
}

//! This overload the resize method on Widget and simply requests a window resize
//! on the windowmanager/OS. The resized() method is called by the event handler
//! once the window has been resized.
void Window::resize(std::size_t width, std::size_t height)
{
	native->resize(width, height);
}

//! This overload the move method on Widget and simply requests a window move
//! on the windowmanager/OS. The moved() method is called by the event handler
//! once the window has been moved.
void Window::move(int x, int y)
{
	native->move(x, y);
}

void Window::show()
{
	Widget::show();
	redraw();
	native->show();
}

void Window::hide()
{
	native->hide();
	Widget::hide();
}

Window* Window::window()
{
	return this;
}

ImageCache& Window::getImageCache()
{
	return image_cache;
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
		oldFocusWidget->redraw();
	}

	if(_keyboardFocus)
	{
		_keyboardFocus->redraw();
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

void Window::needsRedraw()
{
	needs_redraw = true;
}

std::size_t Window::translateToWindowX()
{
	return 0;
}

std::size_t Window::translateToWindowY()
{
	return 0;
}

//! Called by event handler when an windowmanager/OS window resize event has
//! been received. Do not call this directly.
void Window::resized(std::size_t width, std::size_t height)
{
	auto size = native->getSize();
	if((wpixbuf.width == size.first) &&
	   (wpixbuf.height == size.second))
	{
		return;
	}
	wpixbuf.realloc(size.first, size.second);
	Widget::resize(size.first, size.second);
	//updateBuffer();
}

//! Called by event handler when an windowmanager/OS window move event has
//! been received. Do not call this directly.
void Window::moved(int x, int y)
{
	// Make sure widget coordinates are updated.
	Widget::move(x, y);
}

bool Window::updateBuffer()
{
	if(!native)
	{
		return false;
	}

	if(!needs_redraw)
	{
		// Nothing changed, don't update anything.
		return false;
	}

	for(const auto& pixelBuffer : getPixelBuffers())
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

	native->redraw();
	needs_redraw = false;

	return true;
}

} // GUI::

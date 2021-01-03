/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            nativewindow.h
 *
 *  Fri Dec 28 18:46:01 CET 2012
 *  Copyright 2012 Bent Bisballe Nyeng
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
#pragma once

#include <string>
#include <memory>
#include <queue>
#include <tuple>
#include <vector>

#include "guievent.h"

namespace dggui
{

struct Point;

//! Interface class for native window implementations.
class NativeWindow
{
public:
	NativeWindow() {}
	virtual ~NativeWindow() {}

	//! Set a fixed size to the window.
	//! It resizes the window and disallows user resizing.
	virtual void setFixedSize(std::size_t width, std::size_t height) = 0;

	//! Force window to stay on top of other windows
	virtual void setAlwaysOnTop(bool always_on_top) = 0;

	//! Set a new size of the window.
	virtual void resize(std::size_t width, std::size_t height) = 0;

	//! Query size of the native window.
	virtual std::pair<std::size_t, std::size_t> getSize() const = 0;

	//! Move the window to a new position.
	//! Note: negative value are allowed.
	virtual void move(int x, int y) = 0;

	//! Query the screen position of the native window.
	//! Note: returned values can be negative.
	virtual std::pair<int, int> getPosition() const = 0;

	//! Show the window if it is hidden.
	virtual void show() = 0;

	//! Hides the window.
	virtual void hide() = 0;

	//! Return visibility state of the native window.
	virtual bool visible() const = 0;

	//! Sets the window caption in the title bar (if it has one).
	virtual void setCaption(const std::string &caption) = 0;

	//! Draw the internal rendering buffer to the window buffer.
	virtual void redraw(const Rect& dirty_rect) = 0;

	//! Toggle capture mouse mode.
	virtual void grabMouse(bool grab) = 0;

	//! Reads all currently enqueued events from the native window system.
	//! \return A queue of shared pointers to events.
	virtual EventQueue getEvents() = 0;

	//! \returns the native window handle, it HWND on Win32 or Window id on X11
	virtual void* getNativeWindowHandle() const = 0;

	//! Translate a the local native window coordinate to a global screen
	//! coordinate.
	virtual Point translateToScreen(const Point& point) = 0;
};

} // dggui::

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            nativewindow_x11.h
 *
 *  Fri Dec 28 18:45:56 CET 2012
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

#include <queue>

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

#include "nativewindow.h"

namespace GUI
{

class Window;

class NativeWindowX11
	: public NativeWindow
{
public:
	NativeWindowX11(void* native_window, Window& window);
	~NativeWindowX11();

	// From NativeWindow:
	void setFixedSize(std::size_t width, std::size_t height) override;
	void setAlwaysOnTop(bool always_on_top) override;
	void resize(std::size_t width, std::size_t height) override;
	std::pair<std::size_t, std::size_t> getSize() const override;
	void move(int x, int y) override;
	std::pair<int, int> getPosition() const override;
	void show() override;
	void hide() override;
	bool visible() const override;
	void setCaption(const std::string &caption) override;
	void redraw(const Rect& dirty_rect) override;
	void grabMouse(bool grab) override;
	EventQueue getEvents() override;
	void* getNativeWindowHandle() const override;
	Point translateToScreen(const Point& point) override;

private:
	void translateXMessage(XEvent& xevent);

	//! Allocate new shared memory buffer for the pixel buffer.
	//! Frees the existing buffer if there is one.
	void allocateShmImage(std::size_t width, std::size_t height);

	//! Deallocate image and shm resources.
	void deallocateShmImage();

	//! Copy data from the pixel buffer into the shared memory
	void updateImageFromBuffer(std::size_t x1, std::size_t y1,
	                           std::size_t x2, std::size_t y2);

	XShmSegmentInfo shm_info;
	XImage* image{nullptr};

	::Window xwindow{0};
	GC gc{0};

	Window& window;

	Time last_click{0};

	Display* display{nullptr};
	int screen{0};
	int depth{0};
	Visual* visual{nullptr};
	Atom wmDeleteMessage{0};
	::Window parent_window;

	EventQueue event_queue;
};

} // GUI::

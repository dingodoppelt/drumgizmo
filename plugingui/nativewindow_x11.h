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
#pragma once

#include <X11/Xlib.h>

#include "nativewindow.h"

namespace GUI {

class Window;
class NativeWindowX11 : public NativeWindow {
public:
	NativeWindowX11(Window& window);
	~NativeWindowX11();

	// From NativeWindow:
	void setFixedSize(int width, int height) override;
	void resize(int width, int height) override;
	void move(int x, int y) override;
	void show() override;
	void hide() override;
	void setCaption(const std::string &caption) override;
	void handleBuffer() override;
	void redraw() override;
	void grabMouse(bool grab) override;
	bool hasEvent() override;
	Event* getNextEvent() override;
	Event* peekNextEvent() override;

private:
	Event* translateXMessage(XEvent& xevent, bool peek = false);
	XImage* createImageFromBuffer(unsigned char* buf, int width, int height);

	::Window xwindow{0};
	GC gc{0};
	XImage* buffer{nullptr};

	Window& window;

	int last_click{0};

	Display* display{nullptr};
	int screen{0};
	Atom wmDeleteMessage{0};
};

} // GUI::

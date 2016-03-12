/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            window.h
 *
 *  Sun Oct  9 13:11:52 CEST 2011
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
#pragma once

#include "widget.h"

#include "pixelbuffer.h"
#include "nativewindow.h"
#include "image.h"
#include "eventhandler.h"

namespace GUI {

class Window : public Widget {
public:
	Window(void* native_window);
	~Window();

	void setFixedSize(int width, int height);
	void setCaption(const std::string& caption);

	// From Widget:
	void resize(int width, int height) override;
	void move(size_t x, size_t y) override;
	size_t windowX() override;
	size_t windowY() override;
	void show() override;
	void hide() override;
	Window* window() override;

	EventHandler* eventHandler();

	Widget* keyboardFocus();
	void setKeyboardFocus(Widget* widget);

	Widget* buttonDownFocus();
	void setButtonDownFocus(Widget* widget);

	Widget* mouseFocus();
	void setMouseFocus(Widget* widget);

protected:
	// For the EventHandler
	friend class EventHandler;
	void redraw();
	void resized(size_t w, size_t h);
	void updateBuffer();

	// For the Painter
	friend class Painter;
	void beginPaint();
	void endPaint();

	// For the NativeWindow
	friend class NativeWindowX11;
	friend class NativeWindowWin32;
	friend class NativeWindowPugl;
	PixelBuffer wpixbuf;

	size_t refcount{0};

	Widget* _keyboardFocus{nullptr};
	Widget* _buttonDownFocus{nullptr};
	Widget* _mouseFocus{nullptr};

	NativeWindow* native{nullptr};
	EventHandler* eventhandler{nullptr};

	size_t maxRefcount{0};
};

} // GUI::

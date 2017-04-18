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

#include "widget.h"

#include "pixelbuffer.h"
#include "nativewindow.h"
#include "image.h"
#include "eventhandler.h"
#include "imagecache.h"

namespace GUI
{

class Window
	: public Widget
{
public:
	Window(void* native_window = nullptr);
	~Window();

	void setFixedSize(int width, int height);
	void setCaption(const std::string& caption);

	// From Widget:
	void resize(std::size_t width, std::size_t height) override;
	void move(int x, int y) override;
	void show() override;
	void hide() override;
	bool visible() const override;
	Window* window() override;

	ImageCache& getImageCache() override;

	EventHandler* eventHandler();

	Widget* keyboardFocus();
	void setKeyboardFocus(Widget* widget);

	Widget* buttonDownFocus();
	void setButtonDownFocus(Widget* widget);

	Widget* mouseFocus();
	void setMouseFocus(Widget* widget);

	//! Tag the window buffer dirty to be rendered.
	void needsRedraw();

	// \returns the native window handle, it HWND on Win32 or Window id on X11
	void* getNativeWindowHandle() const;

protected:
	// For the EventHandler
	friend class EventHandler;

	// From Widget:
	std::size_t translateToWindowX() override;
	std::size_t translateToWindowY() override;
	void resized(std::size_t width, std::size_t height);
	void moved(int x, int y);

	//! Returns true if window pixel buffer changed and needs to be copied to
	//! native window.
	bool updateBuffer();

	// For the Painter
	friend class Widget;

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

	bool needs_redraw{false};
	ImageCache image_cache;
};

} // GUI::

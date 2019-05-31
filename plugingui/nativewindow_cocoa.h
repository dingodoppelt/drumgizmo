/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            nativewindow_cocoa.h
 *
 *  Sun Dec  4 15:55:14 CET 2016
 *  Copyright 2016 Bent Bisballe Nyeng
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

#include <memory>

#include "nativewindow.h"

namespace GUI
{

class Window;
class NativeWindowCocoa
	: public NativeWindow
{
public:
	NativeWindowCocoa(void* native_window, Window& window);
	~NativeWindowCocoa();

	// From NativeWindow:
	virtual void setFixedSize(std::size_t width, std::size_t height) override;
	virtual void resize(std::size_t width, std::size_t height) override;
	virtual std::pair<std::size_t, std::size_t> getSize() const override;
	virtual void move(int x, int y) override;
	virtual std::pair<int, int> getPosition() const override;
	virtual void show() override;
	virtual void hide() override;
	virtual bool visible() const override;
	virtual void setCaption(const std::string &caption) override;
	virtual void redraw(const Rect& dirty_rect) override;
	virtual void grabMouse(bool grab) override;
	virtual EventQueue getEvents() override;
	virtual void* getNativeWindowHandle() const override;

	// Expose friend members of Window to ObjC++ implementation.
	class Window& getWindow();
	class PixelBuffer& getWindowPixbuf();
	void resized();
	void pushBackEvent(std::shared_ptr<Event> event);

private:
	void updateLayerOffset();

	Window& window;
	std::unique_ptr<struct priv> priv;
	EventQueue event_queue;
	void* native_window{nullptr};
	bool first{true};
};

} // GUI::

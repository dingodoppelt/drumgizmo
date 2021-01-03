/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            nativewindow_pugl.h
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

#include "nativewindow.h"
extern "C"
{
#include <pugl/pugl.h>
}

#include <list>

namespace dggui
{

class Event;
class Window;

class NativeWindowPugl : public NativeWindow {
public:
	NativeWindowPugl(void* native_window, Window& window);
	~NativeWindowPugl();

	void setFixedSize(std::size_t width, std::size_t height) override;
	void resize(std::size_t width, std::size_t height) override;
	std::pair<std::size_t, std::size_t> getSize() const override;

	void move(int x, int y) override;
	std::pair<int, int> getPosition() const override{ return {}; }

	void show() override;
	void setCaption(const std::string &caption) override;
	void hide() override;
	bool visible() const override;
	void redraw(const Rect& dirty_rect) override;
	void grabMouse(bool grab) override;

	EventQueue getEvents() override;

	void* getNativeWindowHandle() const override;

private:
	Window& window;
	PuglView* view{nullptr};

	std::list<Event*> eventq;

	// Internal pugl c-callbacks
	static void onEvent(PuglView* view, const PuglEvent* event);
	static void onReshape(PuglView* view, int width, int height);
	static void onDisplay(PuglView* view);
	static void onMouse(PuglView* view, int button, bool press, int x, int y);
	static void onKeyboard(PuglView* view, bool press, uint32_t key);

	EventQueue event_queue;
	std::uint32_t last_click{0};
};

} // dggui::

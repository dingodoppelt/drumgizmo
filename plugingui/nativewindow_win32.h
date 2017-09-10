/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            nativewindow_win32.h
 *
 *  Fri Dec 28 18:45:51 CET 2012
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

#include "nativewindow.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
typedef HWND WNDID;

namespace GUI {

class Window;
class Event;

class NativeWindowWin32 : public NativeWindow {
public:
	NativeWindowWin32(void* native_window, Window& window);
	~NativeWindowWin32();

	void setFixedSize(std::size_t width, std::size_t height) override;
	void resize(std::size_t width, std::size_t height) override;
	std::pair<std::size_t, std::size_t> getSize() const override;
	void move(int x, int y) override;
	std::pair<int, int> getPosition() const override;
	void show() override;
	bool visible() const override;
	void hide() override;
	void redraw(const Rect& dirty_rect) override;
	void setCaption(const std::string &caption) override;
	void grabMouse(bool grab) override;
	EventQueue getEvents() override;
	void* getNativeWindowHandle() const override;

private:
	static LRESULT CALLBACK dialogProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	static LRESULT CALLBACK subClassProc(HWND hwnd, UINT msg, WPARAM wp,
	                                     LPARAM lp, UINT_PTR id, DWORD_PTR data);

	HWND parent_window;
	Window& window;
	WNDID m_hwnd = 0;
	char* m_className = nullptr;
	EventQueue event_queue;
};

} // GUI::

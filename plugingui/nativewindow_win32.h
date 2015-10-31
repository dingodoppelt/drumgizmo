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
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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

#include "nativewindow.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
typedef HWND WNDID;

namespace GUI {

class Window;
class Event;

class NativeWindowWin32 : public NativeWindow {
public:
	NativeWindowWin32(Window *window);
	~NativeWindowWin32();

	void setFixedSize(int width, int height) override;
	void resize(int width, int height) override;
	void move(int x, int y) override;
	void show() override;
	void setCaption(const std::string &caption) override;
	void hide() override;
	void handleBuffer() override;
	void redraw() override;
	void grabMouse(bool grab) override;

	bool hasEvent() override;
	Event *getNextEvent() override;

private:
	static LRESULT CALLBACK dialogProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	// Needed by dialogProc
	Window* window;
	WNDID m_hwnd = 0;
	char* m_className = nullptr;
	Event* event = nullptr;
};

} // GUI::

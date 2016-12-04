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
	std::shared_ptr<Event> getNextEvent() override;
	std::shared_ptr<Event> peekNextEvent() override;

private:
	Window& window;
};

} // GUI::

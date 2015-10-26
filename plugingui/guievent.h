/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            event.h
 *
 *  Sun Oct  9 16:11:47 CEST 2011
 *  Copyright 2011 Bent Bisballe Nyeng
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

#include <unistd.h>

#include <string>

#ifdef X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif/*X11*/

namespace GUI {

class Event {
public:
	typedef enum {
		MouseMove,
		Repaint,
		Button,
		Scroll,
		Key,
		Close,
		Resize
	} Type;
	virtual ~Event() {}

	virtual Type type() = 0;

#ifdef X11
	::Window window_id;
#endif/*X11*/
};

class MouseMoveEvent : public Event {
public:
	Type type() { return MouseMove; }

	int x;
	int y;
};

class ButtonEvent : public Event {
public:
	Type type() { return Button; }

	int x;
	int y;

	enum {
		Up,
		Down,
	} direction;

	enum {
		Right,
		Middle,
		Left,
	} button;

	bool doubleclick;
};

class ScrollEvent : public Event {
public:
	Type type() { return Scroll; }

	int x;
	int y;

	int delta;
};

class RepaintEvent : public Event {
public:
	Type type() { return Repaint; }

	int x;
	int y;
	size_t width;
	size_t height;
};

class KeyEvent : public Event {
public:
	Type type() { return Key; }

	enum {
		Up,
		Down,
	} direction;

	int keycode;
	std::string text;

	enum {
		KeyUnknown   = -1,
		KeyLeft      =  1,
		KeyRight     =  2,
		KeyUp        =  3,
		KeyDown      =  4,
		KeyDelete    =  5,
		KeyBackspace =  6,
		KeyHome      =  7,
		KeyEnd       =  8,
		KeyPageDown  =  9,
		KeyPageUp    =  10,
		KeyEnter     =  11,
		KeyCharacter =  0xffff // character data is stored in 'text'
	};
};

class CloseEvent : public Event {
public:
	Type type() { return Close; }
};

class ResizeEvent : public Event {
public:
	Type type() { return Resize; }

	size_t width;
	size_t height;
};

} // GUI::

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

enum class EventType {
	mouseMove,
	repaint,
	button,
	scroll,
	key,
	close,
	resize
};

class Event {
public:
	virtual ~Event() {}

	virtual EventType type() = 0;

#ifdef X11
	::Window window_id;
#endif/*X11*/
};

class MouseMoveEvent : public Event {
public:
	EventType type() { return EventType::mouseMove; }

	int x;
	int y;
};


enum class Direction {
	up,
	down,
};

enum class MouseButton {
	right,
	middle,
	left,
};

class ButtonEvent : public Event {
public:
	EventType type() { return EventType::button; }

	int x;
	int y;

	Direction direction;
	MouseButton button;

	bool doubleClick;
};

class ScrollEvent : public Event {
public:
	EventType type() { return EventType::scroll; }

	int x;
	int y;

	int delta;
};

class RepaintEvent : public Event {
public:
	EventType type() { return EventType::repaint; }

	int x;
	int y;
	size_t width;
	size_t height;
};

enum class Key {
	unknown,
	left,
	right,
	up,
	down,
	deleteKey,
	backspace,
	home,
	end,
	pageDown,
	pageUp,
	enter,
	character, //!< The actual character is stored in KeyEvent::text
};

class KeyEvent : public Event {
public:
	EventType type() { return EventType::key; }

	Direction direction;

	Key keycode;
	std::string text;
};

class CloseEvent : public Event {
public:
	EventType type() { return EventType::close; }
};

class ResizeEvent : public Event {
public:
	EventType type() { return EventType::resize; }

	size_t width;
	size_t height;
};

} // GUI::

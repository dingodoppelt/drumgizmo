/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            nativewindow.h
 *
 *  Fri Dec 28 18:46:01 CET 2012
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

#include <string>

#include "guievent.h"

namespace GUI {

//! \brief Interface class for native window implementations.
class NativeWindow {
public:
	NativeWindow() {}
	virtual ~NativeWindow() {}

	//! \brief Set a fixed size to the window.
	//! It resizes the window and disallows user resizing.
	virtual void setFixedSize(int width, int height) = 0;

	// TODO: setScalable(bool) ??

	//! \brief Set a new size of the window.
	virtual void resize(int width, int height) = 0;

	//! \brief Move the window to a new position.
	virtual void move(int x, int y) = 0;

	//! \brief Show the window if it is hidden.
	virtual void show() = 0;

	//! \brief Hides the window.
	virtual void hide() = 0;

	//! \brief Sets the window caption in the title bar (if it has one).
	virtual void setCaption(const std::string &caption) = 0;

	//! \brief Recreate a window render buffer based on the internal buffer.
	//! This need to be called whenever the internal buffer size has changed.
	virtual void handleBuffer() = 0;

	//! \brief Draw the internal rendering buffer to the window buffer.
	virtual void redraw() = 0;

	//! \brief Toggle capture mouse mode.
	virtual void grabMouse(bool grab) = 0;

	//! \brief Query if the event queue contains any events.
	virtual bool hasEvent() = 0;

	//! \brief Read a single event from the event queue.
	//! \return A pointer to the event or nullptr is none exists.
	virtual Event *getNextEvent() = 0;

	//! \brief Read next event without popping it from the event queue.
	//! \return A pointer to the event or nullptr is none exists.
	virtual Event* peekNextEvent() = 0;
};

} // GUI::

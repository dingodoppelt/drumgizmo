/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            eventhandler.h
 *
 *  Sun Oct  9 18:58:29 CEST 2011
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

#include <notifier.h>
#include <memory>
#include <queue>
#include <list>

#include "guievent.h"
#include "nativewindow.h"

namespace GUI
{

class Window;
class Dialog;

class EventHandler
{
public:
	EventHandler(NativeWindow& nativeWindow, Window& window);

	//! Process all events currently in the event queue.
	void processEvents();

	//! Query if any events are currently in the event queue.
	bool hasEvent();

	//! Query if the topmost event in the event queue is of type.
	bool queryNextEventType(EventType type);

	//! Get a single event from the event queue.
	//! \return A pointer to the event or nullptr if there are none.
	std::shared_ptr<Event> getNextEvent();

	void registerDialog(Dialog* dialog);
	void unregisterDialog(Dialog* dialog);

	Notifier<> closeNotifier;

private:
	Window& window;
	NativeWindow& nativeWindow;

	// Used to ignore mouse button release after a double click.
	bool lastWasDoubleClick;

	EventQueue events;

	std::list<Dialog*> dialogs;
};

} // GUI::

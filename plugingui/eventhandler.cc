/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            eventhandler.cc
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
#include "eventhandler.h"

#include "window.h"
#include "painter.h"

namespace GUI
{

EventHandler::EventHandler(NativeWindow& nativeWindow, Window& window)
	: window(window)
	, nativeWindow(nativeWindow)
//	, last_click(0)
	, lastWasDoubleClick(false)
{}

bool EventHandler::hasEvent()
{
	return !events.empty();
}

std::shared_ptr<Event> EventHandler::getNextEvent()
{
	if(events.empty())
	{
		return nullptr;
	}

	auto event = events.front();
	events.pop_front();
	return event;
}

void EventHandler::processEvents()
{
	events = nativeWindow.getEvents();

	while(hasEvent())
	{
		auto event = getNextEvent();

		if(event == nullptr)
		{
			continue;
		}

		switch(event->type()) {
		case EventType::repaint:
			window.redraw();
			break;

		case EventType::move:
			{
				auto moveEvent = static_cast<MoveEvent*>(event.get());
				window.moved(moveEvent->x, moveEvent->y);
			}
			break;

		case EventType::resize:
			{
				auto resizeEvent = static_cast<ResizeEvent*>(event.get());
				if((resizeEvent->width != window.width()) ||
				   (resizeEvent->height != window.height()))
				{
					window.resized(resizeEvent->width, resizeEvent->height);
				}
			}
			break;

		case EventType::mouseMove:
			{
				while(true)
				{
					if(!hasEvent())
					{
						break;
					}

					event = getNextEvent();
				}

				auto moveEvent = static_cast<MouseMoveEvent*>(event.get());

				auto widget = window.find(moveEvent->x, moveEvent->y);
				auto oldwidget = window.mouseFocus();
				if(widget != oldwidget)
				{
					// Send focus leave to oldwidget
					if(oldwidget)
					{
						oldwidget->mouseLeaveEvent();
					}

					// Send focus enter to widget
					if(widget)
					{
						widget->mouseEnterEvent();
					}

					window.setMouseFocus(widget);
				}

				if(window.buttonDownFocus())
				{
					auto widget = window.buttonDownFocus();
					moveEvent->x -= widget->translateToWindowX();
					moveEvent->y -= widget->translateToWindowY();

					window.buttonDownFocus()->mouseMoveEvent(moveEvent);
					break;
				}

				if(widget)
				{
					moveEvent->x -= widget->translateToWindowX();
					moveEvent->y -= widget->translateToWindowY();
					widget->mouseMoveEvent(moveEvent);
				}
			}
			break;

		case EventType::button:
			{
				auto buttonEvent = static_cast<ButtonEvent*>(event.get());

				if(lastWasDoubleClick && (buttonEvent->direction == Direction::down))
				{
					lastWasDoubleClick = false;
					continue;
				}

				lastWasDoubleClick = buttonEvent->doubleClick;

				auto widget = window.find(buttonEvent->x, buttonEvent->y);

				if(window.buttonDownFocus())
				{
					if(buttonEvent->direction == Direction::up)
					{
						auto widget = window.buttonDownFocus();
						buttonEvent->x -= widget->translateToWindowX();
						buttonEvent->y -= widget->translateToWindowY();

						widget->buttonEvent(buttonEvent);
						window.setButtonDownFocus(nullptr);
						break;
					}
				}

				if(widget)
				{
					buttonEvent->x -= widget->translateToWindowX();
					buttonEvent->y -= widget->translateToWindowY();

					widget->buttonEvent(buttonEvent);

					if((buttonEvent->direction == Direction::down) &&
					   widget->catchMouse())
					{
						window.setButtonDownFocus(widget);
					}

					if(widget->isFocusable())
					{
						window.setKeyboardFocus(widget);
					}
				}
			}
			break;

		case EventType::scroll:
			{
				auto scrollEvent = static_cast<ScrollEvent*>(event.get());

				auto widget = window.find(scrollEvent->x, scrollEvent->y);
				if(widget)
				{
					scrollEvent->x -= widget->translateToWindowX();
					scrollEvent->y -= widget->translateToWindowY();

					widget->scrollEvent(scrollEvent);
				}
			}
			break;

		case EventType::key:
			{

				// TODO: Filter out multiple arrow events.

				auto keyEvent = static_cast<KeyEvent*>(event.get());
				if(window.keyboardFocus())
				{
					window.keyboardFocus()->keyEvent(keyEvent);
				}
			}
			break;

		case EventType::close:
			closeNotifier();
			break;
		}
	}

	// Probe window and children to readrw as needed.
	// NOTE: This method will invoke native->redraw() if a redraw is needed.
	window.updateBuffer();
}

} // GUI::

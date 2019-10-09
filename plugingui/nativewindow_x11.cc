/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            nativewindow_x11.cc
 *
 *  Fri Dec 28 18:45:57 CET 2012
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
#include "nativewindow_x11.h"

//http://www.mesa3d.org/brianp/xshm.c

#include <X11/Xutil.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cerrno>
#include <cstring>
#include <cassert>

#include <chrono>

#include <hugin.hpp>

#include "window.h"

namespace GUI
{

#define _NET_WM_STATE_REMOVE        0    // remove/unset property
#define _NET_WM_STATE_ADD           1    // add/set property

void setWindowFront(Display *disp, ::Window wind, bool enable)
{
	Atom wm_state, wm_state_above;
	XEvent event;

	if((wm_state = XInternAtom(disp, "_NET_WM_STATE", False)) == None)
	{
		return;
	}

	if((wm_state_above = XInternAtom(disp, "_NET_WM_STATE_ABOVE", False)) == None)
	{
		return;
	}
	//
	//window  = the respective client window
	//message_type = _NET_WM_STATE
	//format = 32
	//data.l[0] = the action, as listed below
	//data.l[1] = first property to alter
	//data.l[2] = second property to alter
	//data.l[3] = source indication (0-unk,1-normal app,2-pager)
	//other data.l[] elements = 0
	//

	// sending a ClientMessage
	event.xclient.type = ClientMessage;

	// value unimportant in this case
	event.xclient.serial = 0;

	// coming from a SendEvent request, so True
	event.xclient.send_event = True;

	// the event originates from disp
	event.xclient.display = disp;

	// the window whose state will be modified
	event.xclient.window = wind;

	// the component Atom being modified in the window
	event.xclient.message_type = wm_state;

	// specifies that data.l will be used
	event.xclient.format = 32;

	// 0 is _NET_WM_STATE_REMOVE, 1 is _NET_WM_STATE_ADD
	event.xclient.data.l[0] =
		enable ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;

	// the atom being added
	event.xclient.data.l[1] = wm_state_above;

	// unused
	event.xclient.data.l[2] = 0;
	event.xclient.data.l[3] = 0;
	event.xclient.data.l[4] = 0;

	// actually send the event
	XSendEvent(disp, DefaultRootWindow(disp), False,
	           SubstructureRedirectMask | SubstructureNotifyMask, &event);
}

NativeWindowX11::NativeWindowX11(void* native_window, Window& window)
	: window(window)
{
	display = XOpenDisplay(nullptr);
	if(display  == nullptr)
	{
		ERR(X11, "XOpenDisplay failed");
		return;
	}

	screen = DefaultScreen(display);
	visual = DefaultVisual(display, screen);
	depth = DefaultDepth(display, screen);

	if(native_window)
	{
		parent_window = (::Window)native_window;

		// Track size changes on the parent window
		XSelectInput(display, parent_window, StructureNotifyMask);
	}
	else
	{
		parent_window = DefaultRootWindow(display);
	}

	// Create the window
	XSetWindowAttributes swa;
	swa.backing_store = Always;
	xwindow = XCreateWindow(display,
	                        parent_window,
	                        0, 0, //window.x(), window.y(),
	                        1, 1, //window.width(), window.height(),
	                        0, // border
	                        CopyFromParent, // depth
	                        CopyFromParent, // class
	                        CopyFromParent, // visual
	                        0,//CWBackingStore,
	                        &swa);

	long mask = (StructureNotifyMask |
	             PointerMotionMask |
	             ButtonPressMask |
	             ButtonReleaseMask |
	             KeyPressMask |
	             KeyReleaseMask|
	             ExposureMask |
	             StructureNotifyMask |
	             SubstructureNotifyMask |
	             EnterWindowMask |
	             LeaveWindowMask);
	XSelectInput(display, xwindow, mask);

	// Register the delete window message:
	wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", false);

	Atom protocols[] = { wmDeleteMessage };
	XSetWMProtocols(display, xwindow, protocols,
	                sizeof(protocols) / sizeof(*protocols));

	// Create a "Graphics Context"
	gc = XCreateGC(display, xwindow, 0, nullptr);
}

NativeWindowX11::~NativeWindowX11()
{
	if(display == nullptr)
	{
		return;
	}

	deallocateShmImage();

	XFreeGC(display, gc);

	XDestroyWindow(display, xwindow);
	XCloseDisplay(display);
}

void NativeWindowX11::setFixedSize(std::size_t width, std::size_t height)
{
	if(display == nullptr)
	{
		return;
	}

	resize(width, height);

	XSizeHints size_hints;
	memset(&size_hints, 0, sizeof(size_hints));

	size_hints.flags = PMinSize|PMaxSize;
	size_hints.min_width = size_hints.max_width = (int)width;
	size_hints.min_height = size_hints.max_height = (int)height;

	XSetNormalHints(display, xwindow, &size_hints);
}

void NativeWindowX11::setAlwaysOnTop(bool always_on_top)
{
	setWindowFront(display, xwindow, always_on_top);
}

void NativeWindowX11::resize(std::size_t width, std::size_t height)
{
	if(display == nullptr)
	{
		return;
	}

	XResizeWindow(display, xwindow, width, height);
}

std::pair<std::size_t, std::size_t> NativeWindowX11::getSize() const
{
//	XWindowAttributes attributes;
//	XGetWindowAttributes(display, xwindow, &attributes);
//	return std::make_pair(attributes.width, attributes.height);

	::Window root_window;
	int x, y;
	unsigned int width, height, border, depth;

	XGetGeometry(display, xwindow, &root_window,
	             &x, &y,
	             &width, &height, &border, &depth);

	return {width, height};
}

void NativeWindowX11::move(int x, int y)
{
	if(display == nullptr)
	{
		return;
	}

	XMoveWindow(display, xwindow, x, y);
}

std::pair<int, int> NativeWindowX11::getPosition() const
{
	::Window root_window;
	::Window child_window;
	int x, y;
	unsigned int width, height, border, depth;

	XGetGeometry(display, xwindow, &root_window,
	             &x, &y,
	             &width, &height, &border, &depth);

	XTranslateCoordinates(display, xwindow, root_window,
	                      0, 0, &x, &y, &child_window);

	return std::make_pair(x, y);
}

void NativeWindowX11::show()
{
	if(display == nullptr)
	{
		return;
	}

	XMapWindow(display, xwindow);
}

void NativeWindowX11::hide()
{
	if(display == nullptr)
	{
		return;
	}

	XUnmapWindow(display, xwindow);
}

bool NativeWindowX11::visible() const
{
	if(display == nullptr)
	{
		return false;
	}

	XWindowAttributes xwa;
	XGetWindowAttributes(display, xwindow, &xwa);
	return (xwa.map_state == IsViewable);
}

void NativeWindowX11::redraw(const Rect& dirty_rect)
{
	if(display == nullptr)
	{
		return;
	}

	auto x1 = dirty_rect.x1;
	auto y1 = dirty_rect.y1;
	auto x2 = dirty_rect.x2;
	auto y2 = dirty_rect.y2;

	// Assert that we don't try to paint a backwards rect.
	assert(x1 <= x2);
	assert(y1 <= y2);

	updateImageFromBuffer(x1, y1, x2, y2);

	XShmPutImage(display, xwindow, gc, image, x1, y1, x1, y1,
	             std::min((std::size_t)image->width, (x2 - x1)),
	             std::min((std::size_t)image->height, (y2 - y1)), false);
	XFlush(display);
}

void NativeWindowX11::setCaption(const std::string &caption)
{
	if(display == nullptr)
	{
		return;
	}

	XStoreName(display, xwindow, caption.c_str());
}

void NativeWindowX11::grabMouse(bool grab)
{
	(void)grab;
	// Don't need to do anything on this platform...
}

EventQueue NativeWindowX11::getEvents()
{
	while(XPending(display))
	{
		XEvent xEvent;
		XNextEvent(display, &xEvent);
		translateXMessage(xEvent);
	}

	EventQueue events;
	std::swap(events, event_queue);
	return events;
}

void* NativeWindowX11::getNativeWindowHandle() const
{
	return (void*)xwindow;
}

Point NativeWindowX11::translateToScreen(const Point& point)
{
	::Window child_window;
	Point p;
	XTranslateCoordinates(display, xwindow, DefaultRootWindow(display),
	                      point.x, point.y, &p.x, &p.y, &child_window);
	return p;
}

void NativeWindowX11::translateXMessage(XEvent& xevent)
{
	switch(xevent.type)
	{
	case MotionNotify:
		//DEBUG(x11, "MotionNotify");
		{
			auto mouseMoveEvent = std::make_shared<MouseMoveEvent>();
			mouseMoveEvent->x = xevent.xmotion.x;
			mouseMoveEvent->y = xevent.xmotion.y;
			event_queue.push_back(mouseMoveEvent);
		}
		break;

	case Expose:
		//DEBUG(x11, "Expose");
		if(xevent.xexpose.count == 0)
		{
			auto repaintEvent = std::make_shared<RepaintEvent>();
			repaintEvent->x = xevent.xexpose.x;
			repaintEvent->y = xevent.xexpose.y;
			repaintEvent->width = xevent.xexpose.width;
			repaintEvent->height = xevent.xexpose.height;
			event_queue.push_back(repaintEvent);

			if(image)
			{
				// Redraw the entire window.
				Rect rect{0, 0, window.wpixbuf.width, window.wpixbuf.height};
				redraw(rect);
			}
		}
		break;

	case ConfigureNotify:
		//DEBUG(x11, "ConfigureNotify");

		// The parent window size changed, reflect the new size in our own window.
		if(xevent.xconfigure.window == parent_window)
		{
			resize(xevent.xconfigure.width, xevent.xconfigure.height);
			return;
		}

		{
			if((window.width() != (std::size_t)xevent.xconfigure.width) ||
			   (window.height() != (std::size_t)xevent.xconfigure.height))
			{
				auto resizeEvent = std::make_shared<ResizeEvent>();
				resizeEvent->width = xevent.xconfigure.width;
				resizeEvent->height = xevent.xconfigure.height;
				event_queue.push_back(resizeEvent);
			}

			if((window.x() != xevent.xconfigure.x) ||
			   (window.y() != xevent.xconfigure.y))
			{
				auto moveEvent = std::make_shared<MoveEvent>();
				moveEvent->x = xevent.xconfigure.x;
				moveEvent->y = xevent.xconfigure.y;
				event_queue.push_back(moveEvent);
			}
		}
		break;

	case ButtonPress:
	case ButtonRelease:
		//DEBUG(x11, "ButtonPress");
		{
			if((xevent.xbutton.button == 4) || (xevent.xbutton.button == 5))
			{
				if(xevent.type == ButtonPress)
				{
					int scroll = 1;
					auto scrollEvent = std::make_shared<ScrollEvent>();
					scrollEvent->x = xevent.xbutton.x;
					scrollEvent->y = xevent.xbutton.y;
					scrollEvent->delta = scroll * ((xevent.xbutton.button == 4) ? -1 : 1);
					event_queue.push_back(scrollEvent);
				}
			}
			else if ((xevent.xbutton.button == 6) || (xevent.xbutton.button == 7))
			{
				// Horizontal scrolling case
				// FIXME Introduce horizontal scrolling event to handle this.
			}
			else
			{
				auto buttonEvent = std::make_shared<ButtonEvent>();
				buttonEvent->x = xevent.xbutton.x;
				buttonEvent->y = xevent.xbutton.y;
				switch(xevent.xbutton.button) {
				case 1:
					buttonEvent->button = MouseButton::left;
					break;
				case 2:
					buttonEvent->button = MouseButton::middle;
					break;
				case 3:
					buttonEvent->button = MouseButton::right;
					break;
				default:
					WARN(X11, "Unknown button %d, setting to MouseButton::left\n",
					     xevent.xbutton.button);
					buttonEvent->button = MouseButton::left;
					break;
				}

				buttonEvent->direction =
					(xevent.type == ButtonPress) ?
					Direction::down : Direction::up;

				// This is a fix for hosts (e.g. those using JUCE) that set the
				// event time to '0'.
				if(xevent.xbutton.time == 0)
				{
					auto now = std::chrono::system_clock::now().time_since_epoch();
					xevent.xbutton.time =
						std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
				}

				buttonEvent->doubleClick =
					(xevent.type == ButtonPress) &&
					((xevent.xbutton.time - last_click) < 200);

				if(xevent.type == ButtonPress)
				{
					last_click = xevent.xbutton.time;
				}
				event_queue.push_back(buttonEvent);
			}
		}
		break;

	case KeyPress:
	case KeyRelease:
		//DEBUG(x11, "KeyPress");
		{
			auto keyEvent = std::make_shared<KeyEvent>();

			switch(xevent.xkey.keycode) {
			case 113: keyEvent->keycode = Key::left; break;
			case 114: keyEvent->keycode = Key::right; break;
			case 111: keyEvent->keycode = Key::up; break;
			case 116: keyEvent->keycode = Key::down; break;
			case 119: keyEvent->keycode = Key::deleteKey; break;
			case 22:  keyEvent->keycode = Key::backspace; break;
			case 110: keyEvent->keycode = Key::home; break;
			case 115: keyEvent->keycode = Key::end; break;
			case 117: keyEvent->keycode = Key::pageDown; break;
			case 112: keyEvent->keycode = Key::pageUp; break;
			case 36:  keyEvent->keycode = Key::enter; break;
			default:  keyEvent->keycode = Key::unknown; break;
			}

			char stringBuffer[1024];
			int size = XLookupString(&xevent.xkey, stringBuffer,
		                         sizeof(stringBuffer), nullptr, nullptr);
			if(size && keyEvent->keycode == Key::unknown)
			{
				keyEvent->keycode = Key::character;
			}

			keyEvent->text.append(stringBuffer, size);

			keyEvent->direction =
				(xevent.type == KeyPress) ? Direction::down : Direction::up;

			event_queue.push_back(keyEvent);
		}
		break;

	case ClientMessage:
		//DEBUG(x11, "ClientMessage");
		if(((unsigned int)xevent.xclient.data.l[0] == wmDeleteMessage))
		{
			auto closeEvent = std::make_shared<CloseEvent>();
			event_queue.push_back(closeEvent);
		}
		break;

	case EnterNotify:
		//DEBUG(x11, "EnterNotify");
		{
			auto enterEvent = std::make_shared<MouseEnterEvent>();
			enterEvent->x = xevent.xcrossing.x;
			enterEvent->y = xevent.xcrossing.y;
			event_queue.push_back(enterEvent);
		}
		break;

	case LeaveNotify:
		//DEBUG(x11, "LeaveNotify");
		{
			auto leaveEvent = std::make_shared<MouseLeaveEvent>();
			leaveEvent->x = xevent.xcrossing.x;
			leaveEvent->y = xevent.xcrossing.y;
			event_queue.push_back(leaveEvent);
		}
		break;

	case MapNotify:
	case MappingNotify:
		//DEBUG(x11, "EnterNotify");
		// There's nothing to do here atm.
		break;

	default:
		WARN(X11, "Unhandled xevent.type: %d\n", xevent.type);
		break;
	}
}

void NativeWindowX11::allocateShmImage(std::size_t width, std::size_t height)
{
	DEBUG(x11, "(Re)alloc XShmImage (%d, %d)", (int)width, (int)height);

	if(image)
	{
		deallocateShmImage();
	}

	if(!XShmQueryExtension(display))
	{
		ERR(x11, "XShmExtension not available");
		return;
	}

	image = XShmCreateImage(display, visual, depth,
	                        ZPixmap, nullptr, &shm_info,
	                        width, height);
	if(image == nullptr)
	{
		ERR(x11, "XShmCreateImage failed!\n");
		return;
	}

	std::size_t byte_size = image->bytes_per_line * image->height;

	// Allocate shm buffer
	int shm_id = shmget(IPC_PRIVATE, byte_size, IPC_CREAT|0777);
	if(shm_id == -1)
	{
		ERR(x11, "shmget failed: %s", strerror(errno));
		return;
	}

	shm_info.shmid = shm_id;

	// Attach share memory bufer
	void* shm_addr = shmat(shm_id, nullptr, 0);
	if(reinterpret_cast<long int>(shm_addr) == -1)
	{
		ERR(x11, "shmat failed: %s", strerror(errno));
		return;
	}

	shm_info.shmaddr = reinterpret_cast<char*>(shm_addr);
	image->data = shm_info.shmaddr;
	shm_info.readOnly = false;

	// This may trigger the X protocol error we're ready to catch:
	XShmAttach(display, &shm_info);
	XSync(display, false);

	// Make the shm id unavailable to others
	shmctl(shm_id, IPC_RMID, 0);
}

void NativeWindowX11::deallocateShmImage()
{
	if(image == nullptr)
	{
		return;
	}

	XFlush(display);
	XShmDetach(display, &shm_info);
	XDestroyImage(image);
	image = nullptr;
	shmdt(shm_info.shmaddr);
}

void NativeWindowX11::updateImageFromBuffer(std::size_t x1, std::size_t y1,
                                            std::size_t x2, std::size_t y2)
{
	//DEBUG(x11, "depth: %d", depth);

	auto width = window.wpixbuf.width;
	auto height = window.wpixbuf.height;

	// If image hasn't been allocated yet or if the image backbuffer is
	// too small, (re)allocate with a suitable size.
	if((image == nullptr) ||
	   ((int)width > image->width) ||
	   ((int)height > image->height))
	{
		constexpr std::size_t step_size = 128; // size increments
		std::size_t new_width = ((width / step_size) + 1) * step_size;
		std::size_t new_height = ((height / step_size) + 1) * step_size;
		allocateShmImage(new_width, new_height);
		x1 = 0;
		y1 = 0;
		x2 = width;
		y2 = height;
	}

	auto stride = image->width;

	std::uint8_t* pixel_buffer = (std::uint8_t*)window.wpixbuf.buf;
	if(depth >= 24) // RGB 888 format
	{
		std::uint32_t* shm_addr = (std::uint32_t*)shm_info.shmaddr;

		for(std::size_t y = y1; y < y2; ++y)
		{
			for(std::size_t x = x1; x < x2; ++x)
			{
				const std::size_t pin = y * width + x;
				const std::size_t pout = y * stride + x;
				const std::uint8_t red = pixel_buffer[pin * 3];
				const std::uint8_t green = pixel_buffer[pin * 3 + 1];
				const std::uint8_t blue = pixel_buffer[pin * 3 + 2];
				shm_addr[pout] = (red << 16) | (green << 8) | blue;
			}
		}
	}
	else if(depth >= 15) // RGB 565 format
	{
		std::uint16_t* shm_addr = (std::uint16_t*)shm_info.shmaddr;

		for(std::size_t y = y1; y < y2; ++y)
		{
			for(std::size_t x = x1; x < x2; ++x)
			{
				const std::size_t pin = y * width + x;
				const std::size_t pout = y * stride + x;
				const std::uint8_t red = pixel_buffer[pin * 3];
				const std::uint8_t green = pixel_buffer[pin * 3 + 1];
				const std::uint8_t blue = pixel_buffer[pin * 3 + 2];
				shm_addr[pout] = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
			}
		}
	}
}

} // GUI::

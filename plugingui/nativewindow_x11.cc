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

#include <X11/Xutil.h>
#include <stdlib.h>
#include <chrono>

#include <hugin.hpp>

#include "window.h"

namespace GUI {

NativeWindowX11::NativeWindowX11(void* native_window, Window& window)
	: buffer(nullptr)
	, window(window)
{
	display = XOpenDisplay(nullptr);
	if(display  == nullptr)
	{
		ERR(X11, "XOpenDisplay failed");
		return;
	}

	screen = DefaultScreen(display);

	::Window parentWindow;
	if(native_window)
	{
		parentWindow = (::Window)native_window;
	}
	else
	{
		parentWindow = DefaultRootWindow(display);
	}

	// Create the window
	unsigned long border = 0;
	XSetWindowAttributes swa;
	swa.backing_store = Always;
	xwindow = XCreateWindow(display,
	                        parentWindow,
	                        window.x(), window.y(),
	                        window.width(), window.height(),
	                        border,
	                        CopyFromParent, // depth
	                        CopyFromParent, // class
	                        CopyFromParent, // visual
	                        CWBackingStore,
	                        &swa);

	long mask = (StructureNotifyMask |
	             PointerMotionMask |
	             ButtonPressMask |
	             ButtonReleaseMask |
	             KeyPressMask |
	             KeyReleaseMask|
	             ExposureMask |
	             StructureNotifyMask |
	             SubstructureNotifyMask);
	XSelectInput(display, xwindow, mask);

	// Register the delete window message:
	wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", false);

	Atom protocols[] = { wmDeleteMessage };
	int count = sizeof(protocols)/sizeof(Atom);
	XSetWMProtocols(display, xwindow, protocols, count);

	// Create a "Graphics Context"
	gc = XCreateGC(display, xwindow, 0, nullptr);
}

NativeWindowX11::~NativeWindowX11()
{
	if(display == nullptr)
	{
		return;
	}

	if(buffer)
	{
		XDestroyImage(buffer);
	}

	XFreeGC(display, gc);

	XDestroyWindow(display, xwindow);
	XCloseDisplay(display);
}

void NativeWindowX11::setFixedSize(int width, int height)
{
	if(display == nullptr)
	{
		return;
	}

	resize(width, height);

	XSizeHints* size_hints;
	size_hints = XAllocSizeHints();

	if(size_hints == nullptr)
	{
		ERR(X11,"XMallocSizeHints() failed");
		return;
	}

	size_hints->flags = USPosition | PMinSize | PMaxSize;
	size_hints->min_width = size_hints->max_width = width;
	size_hints->min_height = size_hints->max_height = height;

	//size_hints->min_aspect.x = (float)window.width()/(float)window.height();
	//size_hints->max_aspect.x = (float)window.width()/(float)window.height();
	//size_hints->min_aspect.y = (float)window.width()/(float)window.height();
	//size_hints->max_aspect.y = size_hints->min_aspect.y;

	XSetWMNormalHints(display, xwindow, size_hints);
}

void NativeWindowX11::resize(int width, int height)
{
	if(display == nullptr)
	{
		return;
	}

	XResizeWindow(display, xwindow, width, height);
}

void NativeWindowX11::move(int x, int y)
{
	if(display == nullptr)
	{
		return;
	}

	XMoveWindow(display, xwindow, x, y);
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

static int get_byte_order (void)
{
	union {
		char c[sizeof(short)];
		short s;
	} order;

	order.s = 1;
	if((1 == order.c[0]))
	{
		return LSBFirst;
	}
	else
	{
		return MSBFirst;
	}
}

XImage* NativeWindowX11::createImageFromBuffer(unsigned char* buf,
                                               int width, int height)
{
	int depth;
	XImage* img = nullptr;
	Visual* vis;
	double rRatio;
	double gRatio;
	double bRatio;
	int outIndex = 0;
	int i;
	int numBufBytes = (3 * (width * height));

	depth = DefaultDepth(display, screen);
	vis = DefaultVisual(display, screen);

	rRatio = vis->red_mask / 255.0;
	gRatio = vis->green_mask / 255.0;
	bRatio = vis->blue_mask / 255.0;

	if(depth >= 24)
	{
		size_t numNewBufBytes = (4 * (width * height));
		u_int32_t *newBuf = (u_int32_t *)malloc (numNewBufBytes);

		for(i = 0; i < numBufBytes; ++i)
		{
			unsigned int r, g, b;
			r = (buf[i] * rRatio);
			++i;
			g = (buf[i] * gRatio);
			++i;
			b = (buf[i] * bRatio);

			r &= vis->red_mask;
			g &= vis->green_mask;
			b &= vis->blue_mask;

			newBuf[outIndex] = r | g | b;
			++outIndex;
		}

		img = XCreateImage (display,
		                    CopyFromParent, depth,
		                    ZPixmap, 0,
		                    (char*) newBuf,
		                    width, height,
		                    32, 0);
	}
	else
	{
		if(depth >= 15)
		{
			size_t numNewBufBytes = (2 * (width * height));
			u_int16_t* newBuf = (u_int16_t*)malloc (numNewBufBytes);

			for(i = 0; i < numBufBytes; ++i)
			{
				unsigned int r, g, b;

				r = (buf[i] * rRatio);
				++i;
				g = (buf[i] * gRatio);
				++i;
				b = (buf[i] * bRatio);

				r &= vis->red_mask;
				g &= vis->green_mask;
				b &= vis->blue_mask;

				newBuf[outIndex] = r | g | b;
				++outIndex;
			}

			img = XCreateImage(display, CopyFromParent, depth, ZPixmap, 0,
			                   (char*)newBuf, width, height, 16, 0);
		}
		else
		{
			//fprintf (stderr, "This program does not support displays with a depth less than 15.");
			return nullptr;
		}
	}

	XInitImage (img);

	// Set the client's byte order, so that XPutImage knows what
	// to do with the data.
	// The default in a new X image is the server's format, which
	// may not be what we want.
	if((LSBFirst == get_byte_order ()))
	{
		img->byte_order = LSBFirst;
	}
	else
	{
		img->byte_order = MSBFirst;
	}

	// The bitmap_bit_order doesn't matter with ZPixmap images.
	img->bitmap_bit_order = MSBFirst;

	return img;
}

void NativeWindowX11::handleBuffer()
{
	if(buffer)
	{
		XDestroyImage(buffer);
	}

	buffer = createImageFromBuffer(window.wpixbuf.buf,
	                               window.wpixbuf.width,
	                               window.wpixbuf.height);
}

void NativeWindowX11::redraw()
{
	if(display == nullptr)
	{
		return;
	}

	if(buffer == nullptr)
	{
		window.updateBuffer();
	}

	XPutImage(display, xwindow, gc, buffer, 0, 0, 0, 0,
	          window.width(), window.height());
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

bool NativeWindowX11::hasEvent()
{
	if(display == nullptr)
	{
		return false;
	}

	return XPending(display);
}

std::shared_ptr<Event> NativeWindowX11::getNextEvent()
{
	if(display == nullptr)
	{
		return nullptr;
	}

	XEvent xEvent;
	XNextEvent(display, &xEvent);
	return translateXMessage(xEvent);
}

std::shared_ptr<Event> NativeWindowX11::peekNextEvent()
{
	if(display == nullptr)
	{
		return nullptr;
	}

	XEvent peekXEvent;
	XPeekEvent(display, &peekXEvent);
	return translateXMessage(peekXEvent, true);
}

std::shared_ptr<Event> NativeWindowX11::translateXMessage(XEvent& xevent,
                                                          bool peek)
{
	std::shared_ptr<Event> event = nullptr;

	switch(xevent.type) {
	case MotionNotify:
		{
			auto mouseMoveEvent = std::make_shared<MouseMoveEvent>();
			mouseMoveEvent->window_id = xevent.xmotion.window;
			mouseMoveEvent->x = xevent.xmotion.x;
			mouseMoveEvent->y = xevent.xmotion.y;
			event = mouseMoveEvent;
		}
		break;

	case Expose:
		if(xevent.xexpose.count == 0)
		{
			auto repaintEvent = std::make_shared<RepaintEvent>();
			repaintEvent->window_id = xevent.xexpose.window;
			repaintEvent->x = xevent.xexpose.x;
			repaintEvent->y = xevent.xexpose.y;
			repaintEvent->width = xevent.xexpose.width;
			repaintEvent->height = xevent.xexpose.height;
			event = repaintEvent;
		}
		break;

	case ConfigureNotify:
		{
			if((window.width() != (std::size_t)xevent.xconfigure.width) ||
			   (window.height() != (std::size_t)xevent.xconfigure.height))
			{
				auto resizeEvent = std::make_shared<ResizeEvent>();
				resizeEvent->window_id = xevent.xconfigure.window;
				resizeEvent->width = xevent.xconfigure.width;
				resizeEvent->height = xevent.xconfigure.height;
				event = resizeEvent;
			}
			else if((window.windowX() != (std::size_t)xevent.xconfigure.x) ||
			   (window.windowY() != (std::size_t)xevent.xconfigure.y))
			{
				auto moveEvent = std::make_shared<MoveEvent>();
				moveEvent->window_id = xevent.xconfigure.window;
				moveEvent->x = xevent.xconfigure.x;
				moveEvent->y = xevent.xconfigure.y;
				event = moveEvent;
			}
		}
		break;

	case ButtonPress:
	case ButtonRelease:
		{
			if((xevent.xbutton.button == 4) || (xevent.xbutton.button == 5))
			{
				int scroll = 1;
				auto scrollEvent = std::make_shared<ScrollEvent>();
				scrollEvent->window_id = xevent.xbutton.window;
				scrollEvent->x = xevent.xbutton.x;
				scrollEvent->y = xevent.xbutton.y;
				scrollEvent->delta = scroll * ((xevent.xbutton.button == 4) ? -1 : 1);
				event = scrollEvent;
			}
			else
			{
				auto buttonEvent = std::make_shared<ButtonEvent>();
				buttonEvent->window_id = xevent.xbutton.window;
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

				if(!peek && (xevent.type == ButtonPress))
				{
					last_click = xevent.xbutton.time;
				}
				event = buttonEvent;
			}
		}
		break;

	case KeyPress:
	case KeyRelease:
		{
			auto keyEvent = std::make_shared<KeyEvent>();
			keyEvent->window_id = xevent.xkey.window;

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

			event = keyEvent;
		}
		break;

	case ClientMessage:
		if(((unsigned int)xevent.xclient.data.l[0] == wmDeleteMessage))
		{
			auto closeEvent = std::make_shared<CloseEvent>();
			event = closeEvent;
		}
		break;

	case EnterNotify:
	case LeaveNotify:
	case MapNotify:
		// There's nothing to do here atm.
		break;

	default:
		WARN(X11, "Unhandled xevent.type: %d\n", xevent.type);
		break;
	}

	return event;
}

} // GUI::

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
#include "nativewindow_x11.h"

#include <X11/Xutil.h>
#include <stdlib.h>

#include <hugin.hpp>

#include "window.h"

namespace GUI {

NativeWindowX11::NativeWindowX11(Window* window)
	: NativeWindow()
	, buffer(nullptr)
	, window(window)
{
	display = XOpenDisplay(nullptr);
	if(display  == nullptr)
	{
		ERR(X11, "XOpenDisplay failed");
		return;
	}

	screen = DefaultScreen(display);

	// Get some colors
	int blackColor = BlackPixel(display, screen);

	::Window rootWindow = DefaultRootWindow(display);

	// Create the window
	unsigned long border = 0;
	xwindow = XCreateSimpleWindow(display,
	                              rootWindow,
	                              window->x(), window->y(),
	                              window->width(), window->height(),
	                              border,
	                              blackColor, blackColor);

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

	// "Map" the window (that is, make it appear on the screen)
	XMapWindow(display, xwindow);

	// Create a "Graphics Context"
	gc = XCreateGC(display, xwindow, 0, nullptr);
}

NativeWindowX11::~NativeWindowX11()
{
	if(display == nullptr)
	{
		return;
	}

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

	//size_hints->min_aspect.x = (float)window->width()/(float)window->height();
	//size_hints->max_aspect.x = (float)window->width()/(float)window->height();
	//size_hints->min_aspect.y = (float)window->width()/(float)window->height();
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

	buffer = createImageFromBuffer(window->wpixbuf.buf,
	                               window->wpixbuf.width,
	                               window->wpixbuf.height);
}

void NativeWindowX11::redraw()
{
	if(display == nullptr)
	{
		return;
	}

	if(buffer == nullptr)
	{
		window->updateBuffer();
	}

	XPutImage(display, xwindow, gc, buffer, 0, 0, 0, 0,
	          window->width(), window->height());
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

Event* NativeWindowX11::getNextEvent()
{
	if(display == nullptr)
	{
		return nullptr;
	}

	Event* event = nullptr;

	XEvent xevent;
	XNextEvent(display, &xevent);

	if(xevent.type == MotionNotify)
	{
		while(true) // Hack to make sure only the last event is played.
		{
			if(!hasEvent())
			{
				break;
			}

			XEvent peekXEvent;
			XPeekEvent(display, &peekXEvent);
			if(peekXEvent.type != MotionNotify)
			{
				break;
			}

			XNextEvent(display, &xevent);
		}

		auto mouseMoveEvent = new MouseMoveEvent();
		mouseMoveEvent->window_id = xevent.xmotion.window;
		mouseMoveEvent->x = xevent.xmotion.x;
		mouseMoveEvent->y = xevent.xmotion.y;
		event = mouseMoveEvent;
	}

	if(xevent.type == Expose && xevent.xexpose.count == 0)
	{
		auto repaintEvent = new RepaintEvent();
		repaintEvent->window_id = xevent.xexpose.window;
		repaintEvent->x = xevent.xexpose.x;
		repaintEvent->y = xevent.xexpose.y;
		repaintEvent->width = xevent.xexpose.width;
		repaintEvent->height = xevent.xexpose.height;
		event = repaintEvent;
	}

	if(xevent.type == ConfigureNotify)
	{
		auto resizeEvent = new ResizeEvent();
		resizeEvent->window_id = xevent.xconfigure.window;
		//resizeEvent->x = xevent.xconfigure.x;
		//resizeEvent->y = xevent.xconfigure.y;
		resizeEvent->width = xevent.xconfigure.width;
		resizeEvent->height = xevent.xconfigure.height;
		event = resizeEvent;
	}

	if(xevent.type == ButtonPress || xevent.type == ButtonRelease)
	{
		if((xevent.xbutton.button == 4) || (xevent.xbutton.button == 5))
		{
			int scroll = 1;
			while(true) // Hack to make sure only the last event is played.
			{
				if(!hasEvent())
				{
					break;
				}

				XEvent peekXEvent;
				XPeekEvent(display, &peekXEvent);

				if((peekXEvent.type != ButtonPress) &&
				   (peekXEvent.type != ButtonRelease))
				{
					break;
				}

				scroll += 1;
				XNextEvent(display, &xevent);
			}

			auto scrollEvent = new ScrollEvent();
			scrollEvent->window_id = xevent.xbutton.window;
			scrollEvent->x = xevent.xbutton.x;
			scrollEvent->y = xevent.xbutton.y;
			scrollEvent->delta = scroll * ((xevent.xbutton.button == 4) ? -1 : 1);
			event = scrollEvent;
		}
		else
		{
			auto buttonEvent = new ButtonEvent();
			buttonEvent->window_id = xevent.xbutton.window;
			buttonEvent->x = xevent.xbutton.x;
			buttonEvent->y = xevent.xbutton.y;
			switch(xevent.xbutton.button) {
			case 1:
				buttonEvent->button = ButtonEvent::Left;
				break;
			case 2:
				buttonEvent->button = ButtonEvent::Middle;
				break;
			case 3:
				buttonEvent->button = ButtonEvent::Right;
				break;
			default:
				WARN(X11, "Unknown button %d, setting to Left\n",
				     xevent.xbutton.button);
				buttonEvent->button = ButtonEvent::Left;
				break;
			}

			buttonEvent->direction =
				(xevent.type == ButtonPress) ? ButtonEvent::Down : ButtonEvent::Up;

			buttonEvent->doubleclick =
				(xevent.type == ButtonPress) &&
				((xevent.xbutton.time - last_click) < 200);

			if(xevent.type == ButtonPress)
			{
				last_click = xevent.xbutton.time;
			}
			event = buttonEvent;
		}
	}

	if(xevent.type == KeyPress || xevent.type == KeyRelease)
	{
		//printf("key: %d\n", xevent.xkey.keycode);
		auto keyEvent = new KeyEvent();
		keyEvent->window_id = xevent.xkey.window;

		switch(xevent.xkey.keycode) {
		case 113: keyEvent->keycode = KeyEvent::KeyLeft; break;
		case 114: keyEvent->keycode = KeyEvent::KeyRight; break;
		case 111: keyEvent->keycode = KeyEvent::KeyUp; break;
		case 116: keyEvent->keycode = KeyEvent::KeyDown; break;
		case 119: keyEvent->keycode = KeyEvent::KeyDelete; break;
		case 22:  keyEvent->keycode = KeyEvent::KeyBackspace; break;
		case 110: keyEvent->keycode = KeyEvent::KeyHome; break;
		case 115: keyEvent->keycode = KeyEvent::KeyEnd; break;
		case 117: keyEvent->keycode = KeyEvent::KeyPageDown; break;
		case 112: keyEvent->keycode = KeyEvent::KeyPageUp; break;
		case 36:  keyEvent->keycode = KeyEvent::KeyEnter; break;
		default:  keyEvent->keycode = KeyEvent::KeyUnknown; break;
		}

		char stringBuffer[1024];
		int size = XLookupString(&xevent.xkey, stringBuffer,
		                         sizeof(stringBuffer), nullptr, nullptr);
		if(size && keyEvent->keycode == KeyEvent::KeyUnknown)
		{
			keyEvent->keycode = KeyEvent::KeyCharacter;
		}

		keyEvent->text.append(stringBuffer, size);

		keyEvent->direction =
			(xevent.type == KeyPress) ? KeyEvent::Down : KeyEvent::Up;

		event = keyEvent;
	}

	if((xevent.type == ClientMessage) &&
	   ((unsigned int)xevent.xclient.data.l[0] == wmDeleteMessage))
	{
		auto closeEvent = new CloseEvent();
		event = closeEvent;
	}

	return event;
}

} // GUI::

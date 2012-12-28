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

#ifdef X11
#include <X11/Xutil.h>
#include <stdlib.h>

#include "window.h"

GUI::NativeWindowX11::NativeWindowX11(GlobalContext *gctx, GUI::Window *window)
  : GUI::NativeWindow(gctx)
{
  this->window = window;
  buffer = NULL;

  // Get some colors
  int blackColor = BlackPixel(gctx->display, DefaultScreen(gctx->display));
  
  ::Window w = DefaultRootWindow(gctx->display);

  // Create the window
  xwindow = XCreateSimpleWindow(gctx->display,
                                w,
                                window->x(), window->y(),
                                window->width(), window->height(),
                                0,
                                blackColor, blackColor);

  XSelectInput(gctx->display, xwindow,
               StructureNotifyMask |
               PointerMotionMask |
               ButtonPressMask |
               ButtonReleaseMask |
               KeyPressMask |
               KeyReleaseMask|
               ExposureMask |
               StructureNotifyMask |
               SubstructureNotifyMask);

  // register interest in the delete window message
  gctx->wmDeleteMessage = XInternAtom(gctx->display, "WM_DELETE_WINDOW", false);
  XSetWMProtocols(gctx->display, xwindow, &gctx->wmDeleteMessage, 1);

  // "Map" the window (that is, make it appear on the screen)
  XMapWindow(gctx->display, xwindow);

  // Create a "Graphics Context"
  gc = XCreateGC(gctx->display, xwindow, 0, NULL);
}

GUI::NativeWindowX11::~NativeWindowX11()
{
  XDestroyWindow(gctx->display, xwindow);
  //gctx->widgets.erase(window);
}

void GUI::NativeWindowX11::resize(int width, int height)
{
  XResizeWindow(gctx->display, xwindow, width, height);
}

void GUI::NativeWindowX11::move(int x, int y)
{
  XMoveWindow(gctx->display, xwindow, x, y);
}

void GUI::NativeWindowX11::show()
{
  XMapWindow(gctx->display, xwindow);
}

void GUI::NativeWindowX11::hide()
{
  XUnmapWindow(gctx->display, xwindow);
}

static int get_byte_order (void)
{
	union {
		char c[sizeof(short)];
		short s;
	} order;

	order.s = 1;
	if ((1 == order.c[0])) {
		return LSBFirst;
	} else {
		return MSBFirst;
	}
}

static XImage *create_image_from_buffer(Display *dis, int screen,
                                        unsigned char *buf,
                                        int width, int height)
{
	int depth;
	XImage *img = NULL;
	Visual *vis;
	double rRatio;
	double gRatio;
	double bRatio;
	int outIndex = 0;	
	int i;
	int numBufBytes = (3 * (width * height));
		
	depth = DefaultDepth(dis, screen);
	vis = DefaultVisual(dis, screen);

	rRatio = vis->red_mask / 255.0;
	gRatio = vis->green_mask / 255.0;
	bRatio = vis->blue_mask / 255.0;
		
	if (depth >= 24) {
		size_t numNewBufBytes = (4 * (width * height));
		u_int32_t *newBuf = (u_int32_t *)malloc (numNewBufBytes);
	
		for (i = 0; i < numBufBytes; ++i) {
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
		
		img = XCreateImage (dis, 
			CopyFromParent, depth, 
			ZPixmap, 0, 
			(char *) newBuf,
			width, height,
			32, 0
		);
		
	} else if (depth >= 15) {
		size_t numNewBufBytes = (2 * (width * height));
		u_int16_t *newBuf = (u_int16_t *)malloc (numNewBufBytes);
		
		for (i = 0; i < numBufBytes; ++i) {
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
		
		img = XCreateImage(dis, CopyFromParent, depth, ZPixmap, 0, (char *) newBuf,
                        width, height, 16, 0);
	} else {
		//fprintf (stderr, "This program does not support displays with a depth less than 15.");
		return NULL;				
	}

	XInitImage (img);
	/*Set the client's byte order, so that XPutImage knows what to do with the data.*/
	/*The default in a new X image is the server's format, which may not be what we want.*/
	if ((LSBFirst == get_byte_order ())) {
		img->byte_order = LSBFirst;
	} else {
		img->byte_order = MSBFirst;
	}
	
	/*The bitmap_bit_order doesn't matter with ZPixmap images.*/
	img->bitmap_bit_order = MSBFirst;

	return img;
}		

void GUI::NativeWindowX11::handleBuffer()
{
  if(buffer) XDestroyImage(buffer);
  buffer =
    create_image_from_buffer(gctx->display, DefaultScreen(gctx->display),
                             window->wpixbuf.buf,
                             window->wpixbuf.width,
                             window->wpixbuf.height);
}

void GUI::NativeWindowX11::redraw()
{
  // http://stackoverflow.com/questions/6384987/load-image-onto-a-window-using-xlib
  if(buffer == NULL) window->updateBuffer();
  XPutImage(gctx->display, xwindow, gc, buffer, 0, 0, 0, 0,
            window->width(), window->height());
  XFlush(gctx->display);
}

void GUI::NativeWindowX11::setCaption(const std::string &caption)
{
}

#endif/*X11*/


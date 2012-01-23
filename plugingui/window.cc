/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            window.cc
 *
 *  Sun Oct  9 13:11:53 CEST 2011
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
#include "window.h"

#include "painter.h"

#include "img_back.h"

#ifdef X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif/*X11*/

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

GUI::Window *gwindow = NULL;

#ifdef WIN32
// Delared in eventhandler.cc
LRESULT CALLBACK dialogProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
#endif/*WIN32*/

GUI::Window::Window(GlobalContext *gctx) 
  : Widget(NULL), wpixbuf(640, 200)
{
  gwindow = this;
  
  this->gctx = gctx;

  _x = _y = 100;
  _width = wpixbuf.width;
  _height = wpixbuf.height;

  refcount = 0;
  _keyboardFocus = this;
  _buttonDownFocus = NULL;

#ifdef X11
  buffer = NULL;

  // Get some colors
  int blackColor = BlackPixel(gctx->display, DefaultScreen(gctx->display));
  
  ::Window w = DefaultRootWindow(gctx->display);

  // Create the window
  xwindow = XCreateSimpleWindow(gctx->display,
                                w, _x, _y, _width, _height, 0,
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
#endif/*X11*/

#ifdef WIN32
	WNDCLASSEX wcex;
	WNDID wndId;

	gctx->m_hwnd = 0;
	gctx->m_className = NULL;

	memset(&wcex, 0, sizeof(wcex));
	
	//Time to register a window class.
  //Generic flags and everything. cbWndExtra is the size of a pointer to an
  // object - we need this in the wndproc handler.
	
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;//class_style;
	wcex.lpfnWndProc = (WNDPROC)dialogProc;
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  // Set data:
	wcex.cbWndExtra = sizeof(EventHandler*); // Size of data.
	wcex.hInstance = GetModuleHandle(NULL);

  //	if(ex_style && WS_EX_TRANSPARENT == WS_EX_TRANSPARENT) {
  //		wcex.hbrBackground = NULL;
  //	} else {
  wcex.hbrBackground = NULL;//(HBRUSH) COLOR_BACKGROUND + 1;
  //	}
	
	wcex.lpszClassName = gctx->m_className = strdup("DrumGizmoClass");

	RegisterClassEx(&wcex);

  /*
	if(parent) {
		style = style | WS_CHILD;
		wndId = parent->getWndId();
	} else {
  */
  //style = style | WS_OVERLAPPEDWINDOW;
		wndId = 0;
    //	}

	gctx->m_hwnd = CreateWindowEx(NULL/*ex_style*/, gctx->m_className,
                                "DGBasisWidget",
                                (WS_OVERLAPPEDWINDOW | WS_VISIBLE),
                                _x, _y, _width, _height, wndId, NULL,
                                GetModuleHandle(NULL), NULL);

	SetWindowLong(gctx->m_hwnd, GWL_USERDATA, (LONG)gctx->eventhandler);
#endif/*WIN32*/
}

GUI::Window::~Window()
{
#ifdef X11
  XDestroyWindow(gctx->display, xwindow);
  //gctx->widgets.erase(window);
#endif/*X11*/

#ifdef WIN32
	UnregisterClass(gctx->m_className, GetModuleHandle(NULL));
	free(gctx->m_className);
#endif/*WIN32*/
}

void GUI::Window::repaintEvent(GUI::RepaintEvent *e)
{
  Painter p(this);
  p.drawImage(0, 0, (struct __img__*)&img_back);
}

void GUI::Window::resize(size_t width, size_t height)
{
  //  printf("Window::resize(%d, %d)\n", width, height);

#ifdef X11
  XResizeWindow(gctx->display, xwindow, width, height);
#endif/*X11*/

#ifdef WIN32
  SetWindowPos(gctx->m_hwnd, NULL, -1, -1, (int)width, (int)height + 27,
               SWP_NOMOVE);
#endif/*WIN32*/

  Widget::resize(width, height);
}

void GUI::Window::move(size_t x, size_t y)
{
#ifdef X11
  XMoveWindow(gctx->display, xwindow, x, y);
#endif/*X11*/

#ifdef WIN32
  SetWindowPos(gctx->m_hwnd, NULL, (int)x, (int)y, -1, -1, SWP_NOSIZE);
#endif/*WIN32*/

  // Make sure widget corrds are updated.
  Widget::move(x, y);
}

size_t GUI::Window::x() { return _x; }
size_t GUI::Window::y() { return _y; }
size_t GUI::Window::width() { return _width; }
size_t GUI::Window::height() { return _height; }

void GUI::Window::show()
{
  repaint_r(NULL);
#ifdef X11
  XMapWindow(gctx->display, xwindow);
#endif/*X11*/

#ifdef WIN32
  ShowWindow(gctx->m_hwnd, SW_SHOW);
#endif/*WIN32*/
}

void GUI::Window::hide()
{
#ifdef X11
  XUnmapWindow(gctx->display, xwindow);
#endif/*X11*/

#ifdef WIN32
  ShowWindow(gctx->m_hwnd, SW_HIDE);
#endif/*WIN32*/
}

GUI::Window *GUI::Window::window()
{
  return this;
}

void GUI::Window::beginPaint()
{
  refcount++;
  //  printf("beginPaint(%d)\n", refcount);
}

void GUI::Window::endPaint()
{
  //  printf("endPaint(%d)\n", refcount);
  if(refcount) refcount--;

  if(!refcount) {
    updateBuffer();
#ifdef X11
    //XSendEvent(gctx->display, window, false, ExposureMask, event_send)
#endif/*X11*/
    redraw();
  }
}

#ifdef X11
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
#endif/*X11*/

void GUI::Window::updateBuffer()
{
  //  printf("updateBuffer w:%d h:%d\n", width(), height());

  memset(wpixbuf.buf, 0, wpixbuf.width * wpixbuf.height * 3);

  std::vector<PixelBufferAlpha *> pl = getPixelBuffers();
  std::vector<PixelBufferAlpha *>::iterator pli = pl.begin();
  while(pli != pl.end()) {
    PixelBufferAlpha *pb = *pli;
    //    printf("Buffer idx %d (%d %d) [%d %d]\n", pb->idx, pb->x, pb->y,
    //       pb->width, pb->height);
    for(size_t x = 0; x < pb->width; x++) {
      for(size_t y = 0; y < pb->height; y++) {
        unsigned char r,g,b,a;
        pb->pixel(x,y,&r,&g,&b,&a);
        wpixbuf.setPixel(x + pb->x, y + pb->y, r, g, b, a);
      }
    }
    pli++;
  }

#ifdef X11
  if(buffer) XDestroyImage(buffer);
  buffer =
    create_image_from_buffer(gctx->display, DefaultScreen(gctx->display),
                             wpixbuf.buf, wpixbuf.width, wpixbuf.height);
#endif/*X11*/
}

void GUI::Window::resized(size_t w, size_t h)
{
  _width = w;
  _height = h;
  wpixbuf.realloc(w, h);
  updateBuffer();

  pixbuf.realloc(w, h);
  repaintEvent(NULL);
}

void GUI::Window::redraw()
{
#ifdef X11
  // http://stackoverflow.com/questions/6384987/load-image-onto-a-window-using-xlib
  if(buffer == NULL) updateBuffer();
  XPutImage(gctx->display, xwindow, gc, buffer, 0, 0, 0, 0, width(), height());
  XFlush(gctx->display);
#endif/*X11*/

#ifdef WIN32
  RedrawWindow(gctx->m_hwnd, NULL, NULL, RDW_ERASE|RDW_INVALIDATE);
  UpdateWindow(gctx->m_hwnd);
#endif/*WIN32*/
}

GUI::Widget *GUI::Window::keyboardFocus()
{
  return _keyboardFocus;
}

void GUI::Window::setKeyboardFocus(GUI::Widget *widget)
{
  _keyboardFocus = widget;
  repaint_r(NULL);
}

GUI::Widget *GUI::Window::buttonDownFocus()
{
  return _buttonDownFocus;
}

void GUI::Window::setButtonDownFocus(GUI::Widget *widget)
{
  _buttonDownFocus = widget;
  //  repaint_r(NULL);
}


#ifdef TEST_WINDOW
//Additional dependency files
//deps:
//Required cflags (autoconf vars may be used)
//cflags:
//Required link options (autoconf vars may be used)
//libs:
#include "test.h"

TEST_BEGIN;

// TODO: Put some testcode here (see test.h for usable macros).

TEST_END;

#endif/*TEST_WINDOW*/

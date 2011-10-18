/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            widget.cc
 *
 *  Sun Oct  9 13:01:44 CEST 2011
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
#include "widget.h"

#include "globalcontext.h"
#include "widgetcontext.h"

#include <stdio.h>

#include <X11/Xatom.h>

Widget::Widget(GlobalContext *gctx, Widget *parent)
{
  this->gctx = gctx;
  this->parent = parent;
  wctx = new WidgetContext();

  _x = _y = _width = _height = 10;

#ifdef X11

  // Get some colors
  int blackColor = BlackPixel(gctx->display, DefaultScreen(gctx->display));
  
  Window w;
  if(parent == NULL) w = DefaultRootWindow(gctx->display);
  else w = parent->wctx->window;
  

  // Create the window
  wctx->window = XCreateSimpleWindow(gctx->display,
                                     w, _x, _y, _width, _height, 0,
                                     blackColor, blackColor);

  gctx->widgets[wctx->window] = this;

  // We want to get MapNotify events
  XSelectInput(gctx->display, wctx->window,
               StructureNotifyMask |
               PointerMotionMask |
               ButtonPressMask |
               ButtonReleaseMask |
               KeyPressMask |
               KeyReleaseMask|
               ExposureMask |
               StructureNotifyMask);

  /*
  // register interest in the delete window message
  wdg->wmDeleteMessage = XInternAtom(wdg->dpy, "WM_DELETE_WINDOW", false);
  XSetWMProtocols(wdg->dpy, wdg->w, &wdg->wmDeleteMessage, 1);
  */

  // "Map" the window (that is, make it appear on the screen)
  XMapWindow(gctx->display, wctx->window);

  // Create a "Graphics Context"
  wctx->gc = XCreateGC(gctx->display, wctx->window, 0, NULL);

  // Wait for the MapNotify event
  for(;;) {
    XEvent e;
    XNextEvent(gctx->display, &e);
    if(e.type == MapNotify) break;
  }
#endif/*X11*/

#ifdef WIN32
	WNDCLASSEX wcex;
	WNDID wndId;

	wctx->m_hwnd = 0;
	wctx->m_className = NULL;

	memset(&wcex, 0, sizeof(wcex));

	/*
	Time to register a window class. Generic flags and everything. cbWndExtra is the
	size of a pointer to an object - we need this in the wndproc handler.
	*/
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = class_style;
	wcex.lpfnWndProc = (WNDPROC) data;
	wcex.cbWndExtra = sizeof(CWindow *);
	wcex.hInstance = GetModuleHandle(NULL);

  //	if(ex_style && WS_EX_TRANSPARENT == WS_EX_TRANSPARENT) {
  //		wcex.hbrBackground = NULL;
  //	} else {
  wcex.hbrBackground = (HBRUSH) COLOR_BACKGROUND + 1;
  //	}
	
	wcex.lpszClassName = wctx->m_className = strdup(className);

	RegisterClassEx(&wcex);

	if(parent) {
		style = style | WS_CHILD;
		wndId = parent->getWndId();
	} else {
		style = style | WS_OVERLAPPEDWINDOW;
		wndId = 0;
	}

	wctx->m_hwnd = CreateWindowEx(ex_style, wctx->m_className, "DGBasisWidget", style, x, y, w, h,
		wndId, NULL, GetModuleHandle(NULL), NULL);
#endif/*WIN32*/
}

Widget::~Widget()
{
#ifdef X11
  gctx->widgets.erase(wctx->window);
#endif/*X11*/
  delete wctx;
}

void Widget::show()
{
}

void Widget::hide()
{
}

void Widget::addChild(Widget *widget)
{
  children.push_back(widget);
}

void Widget::setSize(size_t width, size_t height)
{
  _width = width;
  _height = height;
#ifdef X11
  XResizeWindow(gctx->display, wctx->window, width, height);
#endif/*X11*/
}

void Widget::move(size_t x, size_t y)
{
  _x = x;
  _y = y;
#ifdef X11
  XMoveWindow(gctx->display, wctx->window, x, y);
#endif/*X11*/
}

size_t Widget::x() { return _x; }
size_t Widget::y() { return _y; }
size_t Widget::width() { return _width; }
size_t Widget::height() { return _height; }

#ifdef TEST_WIDGET
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

#endif/*TEST_WIDGET*/

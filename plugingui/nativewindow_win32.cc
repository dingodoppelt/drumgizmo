/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            nativewindow_win32.cc
 *
 *  Fri Dec 28 18:45:52 CET 2012
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
#include "nativewindow_win32.h"

#ifdef WIN32

#include "window.h"

// Delared in eventhandler.cc
LRESULT CALLBACK dialogProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

GUI::NativeWindowWin32::NativeWindowWin32(GlobalContext *gctx,
                                          GUI::Window *window)
  : GUI::NativeWindow(gctx)
{
  this->window = window;

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
                                window->x(), window->y(),
                                window->width(), window->height(),
                                wndId, NULL,
                                GetModuleHandle(NULL), NULL);

	SetWindowLong(gctx->m_hwnd, GWL_USERDATA, (LONG)gctx->eventhandler);
}

GUI::NativeWindowWin32::~NativeWindowWin32()
{
	UnregisterClass(gctx->m_className, GetModuleHandle(NULL));
	free(gctx->m_className);
}

void GUI::NativeWindowWin32::resize(int width, int height)
{
  SetWindowPos(gctx->m_hwnd, NULL, -1, -1, (int)width, (int)height + 27,
               SWP_NOMOVE);
}

void GUI::NativeWindowWin32::move(int x, int y)
{
  SetWindowPos(gctx->m_hwnd, NULL, (int)x, (int)y, -1, -1, SWP_NOSIZE);
}

void GUI::NativeWindowWin32::show()
{
  ShowWindow(gctx->m_hwnd, SW_SHOW);
}

void GUI::NativeWindowWin32::handleBuffer()
{
}

void GUI::NativeWindowWin32::hide()
{
  ShowWindow(gctx->m_hwnd, SW_HIDE);
}

void GUI::NativeWindowWin32::redraw()
{
  RedrawWindow(gctx->m_hwnd, NULL, NULL, RDW_ERASE|RDW_INVALIDATE);
  UpdateWindow(gctx->m_hwnd);
}

void GUI::NativeWindowWin32::setCaption(const std::string &caption)
{
}

#endif/*WIN32*/

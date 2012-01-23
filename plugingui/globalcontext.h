/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            globalcontext.h
 *
 *  Sun Oct  9 19:16:47 CEST 2011
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
#ifndef __DRUMGIZMO_GLOBALCONTEXT_H__
#define __DRUMGIZMO_GLOBALCONTEXT_H__

#ifdef X11
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif/*X11*/

#ifdef WIN32
//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
typedef HWND WNDID;
namespace GUI { class EventHandler; };
#endif/*WIN32*/

#include <map>

namespace GUI {

class Widget;

class GlobalContext {
public:
  GlobalContext();
  ~GlobalContext();

#ifdef X11
  Display *display;
  Atom wmDeleteMessage;
#endif/*X11*/

#ifdef WIN32
	WNDID	m_hwnd;
	char	*m_className;
  EventHandler *eventhandler;
#endif/*WIN32*/
};

};

#endif/*__DRUMGIZMO_GLOBALCONTEXT_H__*/

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
#endif/*X11*/

#ifdef WIN32
#include <windows.h>
#endif/*WIN32*/

#include <map>

class Widget;

class GlobalContext {
public:
  GlobalContext();
  ~GlobalContext();

  Widget *keyboardFocus;

#ifdef X11
  Display *display;
  std::map<Window, Widget*> widgets;
#endif/*X11*/
};

#endif/*__DRUMGIZMO_GLOBALCONTEXT_H__*/
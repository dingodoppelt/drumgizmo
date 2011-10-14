/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            event.h
 *
 *  Sun Oct  9 16:11:47 CEST 2011
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
#ifndef __DRUMGIZMO_EVENT_H__
#define __DRUMGIZMO_EVENT_H__

#include <unistd.h>

#include <string>

#ifdef X11
#include <X11/Xlib.h>
#endif/*X11*/

class Event {
public:
  typedef enum {
    MouseMove,
    Repaint,
    Button,
    Key
  } Type;

  virtual Type type() = 0;

#ifdef X11
  Window window_id;
#endif/*X11*/
};

class MouseMoveEvent : public Event {
public:
  virtual Type type() { return MouseMove; }

  size_t x;
  size_t y;
};

class ButtonEvent : public Event {
public:
  virtual Type type() { return Button; }

  size_t x;
  size_t y;

  int direction;
  int button;
};

class RepaintEvent : public Event {
public:
  virtual Type type() { return Repaint; }

  size_t x;
  size_t y;
  size_t width;
  size_t height;
};

class KeyEvent : public Event {
public:
  virtual Type type() { return Key; }

  int direction;
  int keycode;
  std::string text;
};

#endif/*__DRUMGIZMO_EVENT_H__*/

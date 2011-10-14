/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            eventhandler.cc
 *
 *  Sun Oct  9 18:58:29 CEST 2011
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
#include "eventhandler.h"

#include "globalcontext.h"

#include <stdio.h>

#ifdef X11
#include <X11/Xutil.h>
#endif/*X11*/

/**
 * KeyPress                2
 * KeyRelease              3
 * ButtonPress             4
 * ButtonRelease           5
 * MotionNotify            6
 * EnterNotify             7
 * LeaveNotify             8
 * FocusIn                 9
 * FocusOut                10
 * KeymapNotify            11
 * Expose                  12
 * GraphicsExpose          13
 * NoExpose                14
 * VisibilityNotify        15
 * CreateNotify            16
 * DestroyNotify           17
 * UnmapNotify             18
 * MapNotify               19
 * MapRequest              20
 * ReparentNotify          21
 * ConfigureNotify         22
 * ConfigureRequest        23
 * GravityNotify           24
 * ResizeRequest           25
 * CirculateNotify         26
 * CirculateRequest        27
 * PropertyNotify          28
 * SelectionClear          29
 * SelectionRequest        30
 * SelectionNotify         31
 * ColormapNotify          32
 * ClientMessage           33
 * MappingNotify           34
 * GenericEvent            35
 * LASTEvent               36      // must be bigger than any event #
 **/

EventHandler::EventHandler(GlobalContext *gctx)
{
  this->gctx = gctx;
}

bool EventHandler::hasEvent()
{
#ifdef X11
  return XPending(gctx->display);
#endif/*X11*/
  return false;
}

Event *EventHandler::getNextEvent()
{
  Event *event = NULL;
#ifdef X11
  XEvent xe;
  XNextEvent(gctx->display, &xe);

  if(xe.type == MotionNotify) {
    MouseMoveEvent *e = new MouseMoveEvent();
    e->window_id = xe.xmotion.window;
    e->x = xe.xmotion.x;
    e->y = xe.xmotion.y;
    event = e;
  }

  if(xe.type == Expose && xe.xexpose.count == 0) {
    RepaintEvent *e = new RepaintEvent();
    e->window_id = xe.xexpose.window;
    e->x = xe.xexpose.x;
    e->y = xe.xexpose.y;
    e->width = xe.xexpose.width;
    e->height = xe.xexpose.height;
    event = e;
  }

  if(xe.type == ButtonPress || xe.type == ButtonRelease) {
    ButtonEvent *e = new ButtonEvent();
    e->window_id = xe.xbutton.window;
    e->x = xe.xbutton.x;
    e->y = xe.xbutton.y;
    e->button = 0;
    e->direction = xe.type == ButtonPress?1:-1;
    event = e;
  }

  if(xe.type == KeyPress || xe.type == KeyRelease) {
    //    printf("key: %d\n", e.xkey.keycode);
    KeyEvent *e = new KeyEvent();
    e->window_id = xe.xkey.window;

    char buf[1024];
    int sz = XLookupString(&xe.xkey, buf, sizeof(buf),  NULL, NULL);

    e->keycode = xe.xkey.keycode;
    e->text.append(buf, sz);
    e->direction = xe.type == KeyPress?1:-1;
    event = e;
  }

#if 0
  printf("event: %d\n", e.type);

  if(e.type == DestroyNotify) {
  }
  if(e.type == ClientMessage &&
     e.xclient.data.l[0] == wdg->wmDeleteMessage) {
  }
#endif

#endif/*X11*/

  return event;
}



#ifdef TEST_EVENTHANDLER
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

#endif/*TEST_EVENTHANDLER*/

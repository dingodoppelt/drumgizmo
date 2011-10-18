/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            painter.cc
 *
 *  Wed Oct 12 19:48:45 CEST 2011
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
#include "painter.h"

#include "globalcontext.h"
#include "widgetcontext.h"

Painter::Painter(GlobalContext *gctx, WidgetContext *wctx)
{
  this->gctx = gctx;
  this->wctx = wctx;
}

Painter::~Painter()
{
  flush();
}

void Painter::setColour(Colour colour)
{
  this->colour = colour;
#ifdef X11
  // Tell the GC we draw using the white color
  XSetForeground(gctx->display, wctx->gc, colour.getColour(gctx));
#endif/*X11*/
}

void Painter::drawLine(int x1, int y1, int x2, int y2)
{
#ifdef X11
  XDrawLine(gctx->display, wctx->window, wctx->gc, x1, y1, x2, y2);
#endif/*X11*/
}

void Painter::drawRectangle(int x1, int y1, int x2, int y2)
{
#ifdef X11
  XDrawRectangle(gctx->display, wctx->window, wctx->gc, x1, y1, x2, y2);
#endif/*X11*/
}

void Painter::drawFilledRectangle(int x1, int y1, int x2, int y2)
{
#ifdef X11
  for(int i = x1; i < x2; i++) {
    for(int j = y1; j < y2; j++) {
      XDrawPoint(gctx->display, wctx->window, wctx->gc, i, j);
    }
  }
#endif/*X11*/
}

void Painter::drawText(int x, int y, std::string text)
{
#ifdef X11
  XTextItem item;
  item.chars = (char*)text.data();
  item.nchars = text.length();
  item.delta = 0;
  item.font = None;

  XDrawText(gctx->display, wctx->window, wctx->gc, x, y, &item, 1);
#endif/*X11*/
}

void Painter::drawPoint(int x, int y)
{
#ifdef X11
  XDrawPoint(gctx->display, wctx->window, wctx->gc, x, y);
#endif/*X11*/
}

void Painter::drawCircle(int x, int y, int r)
{
#ifdef X11
  XDrawArc(gctx->display, wctx->window, wctx->gc, x, y, r, r, 0, 360 * 64);
#endif/*X11*/
}

void Painter::drawFilledCircle(int x, int y, int r)
{
#ifdef X11
  r *= 2;
  for(int s = 1; s < r; s++)
    XDrawArc(gctx->display, wctx->window, wctx->gc,
             x - s / 2, y - s / 2, s, s,
             0, 360 * 64);
#endif/*X11*/
}

void Painter::flush()
{
#ifdef X11
  // Send the "DrawLine" request to the server
  XFlush(gctx->display);
#endif/*X11*/
}

#ifdef TEST_PAINTER
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

#endif/*TEST_PAINTER*/

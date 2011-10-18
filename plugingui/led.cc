/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            led.cc
 *
 *  Sat Oct 15 19:12:33 CEST 2011
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
#include "led.h"

#include "painter.h"

LED::LED(GlobalContext *gctx, Widget *parent)
  : Widget(gctx, parent)
{
  state = true;
}

void LED::setState(bool state)
{
  if(this->state != state) {
    this->state = state;
    repaint(NULL);
  }
}

void LED::repaint(RepaintEvent *e)
{
  size_t h = height() - 1;
  size_t w = width() - 1;

  Painter p(gctx, wctx);
  
  if(state) p.setColour(Colour(0,1,0));
  else p.setColour(Colour(1,0,0));

  size_t size = w / 2;
  if(h / 2 < size) size = h / 2;
  p.drawFilledCircle(w/2, h/2, size);

  p.setColour(Colour(1));
  p.drawFilledCircle(w/3, h/3, size / 6);
}

#ifdef TEST_LED
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

#endif/*TEST_LED*/

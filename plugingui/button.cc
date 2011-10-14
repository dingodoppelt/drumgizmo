/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            button.cc
 *
 *  Sun Oct  9 13:01:56 CEST 2011
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
#include "button.h"

#include "painter.h"

#include <stdio.h>

Button::Button(GlobalContext *gctx, Widget *parent)
  : Widget(gctx, parent)
{
  state = up;
}

void Button::button(ButtonEvent *e)
{
  if(e->direction == 1) {
    state = down;
    repaint(NULL);
  }
  if(e->direction == -1) {
    state = up;
    repaint(NULL);
    clicked();
  }
}

void Button::repaint(RepaintEvent *e)
{
  Painter p(gctx, wctx);
  
  p.setColour(Colour(0.5));
  p.drawFilledRectangle(1,1,width()-1,height()-1);

  p.setColour(Colour(0.1));
  p.drawRectangle(0,0,width()-1,height()-1);

  p.setColour(Colour(0.8));
  if(state == down) {
    p.drawLine(1,1,1,height());
    p.drawLine(1,1,width(),1);
  }
  if(state == up) {
    p.drawLine(width()-2,0, width()-2,height()-2);
    p.drawLine(width()-2,height()-2,0, height()-2);
  }

  p.setColour(Colour(0.9));
  p.drawText(width()/2-(text.length()*3)+(state==up?0:1),
             height()/2+4+(state==up?0:1), text);
}

void Button::setText(std::string text)
{
  this->text = text;
}

#ifdef TEST_BUTTON
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

#endif/*TEST_BUTTON*/

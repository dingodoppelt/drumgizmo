/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            knob.cc
 *
 *  Thu Feb 28 07:37:27 CET 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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
#include "knob.h"

#include "painter.h"

#include <hugin.hpp>
#include <stdio.h>
#include <math.h>

GUI::Knob::Knob(Widget *parent)
  : GUI::Widget(parent)
{
  state = up;

  val = 0.0;
  maximum = 1.0;
  minimum = 0.0;
  mouse_offset_x = 0;

  handler = NULL;
  ptr = NULL;
}

void GUI::Knob::setValue(float v)
{
  val = v;
  if(handler) handler(ptr);
  repaintEvent(NULL);
}

float GUI::Knob::value()
{
  return val;
}

void GUI::Knob::registerClickHandler(void (*handler)(void *), void *ptr)
{
  this->handler = handler;
  this->ptr = ptr;
}

void GUI::Knob::mouseMoveEvent(MouseMoveEvent *e)
{
  if(state == down) {
    /*
    DEBUG(slider, "Knob::mouseMoveEvent(mouse_offset_x: %d,  e->x: %d)\n",
          mouse_offset_x, e->x);
    */
    if(mouse_offset_x == (e->x + -1*e->y)) return;

    float dval = mouse_offset_x - (e->x + -1*e->y);
    val -= dval / 300.0;

    if(val < 0) val = 0;
    if(val > 1) val = 1;

    if(handler) handler(ptr);
    repaintEvent(NULL);

    mouse_offset_x = e->x + -1*e->y;
  }
}

void GUI::Knob::buttonEvent(ButtonEvent *e)
{
  if(e->direction == 1) {
    state = down;
    mouse_offset_x = e->x + -1*e->y;
    //val = maximum / (float)width() * (float)e->x;
    if(handler) handler(ptr);
    repaintEvent(NULL);
  }
  if(e->direction == -1) {
    state = up;
    mouse_offset_x = e->x + -1*e->y;
    //val = maximum / (float)width() * (float)e->x;
    repaintEvent(NULL);
    clicked();
    if(handler) handler(ptr);
  }
}

void GUI::Knob::repaintEvent(GUI::RepaintEvent *e)
{
  //  DEBUG(slider, "Knob::repaintEvent (%f)\n", val);

  Painter p(this);

  float alpha = 0.8;

  p.setColour(Colour(0, 0));
  p.drawFilledRectangle(0,0,width()-1,height()-1);

  if(hasKeyboardFocus()) {
    p.setColour(Colour(0.6, alpha));
  } else {
    p.setColour(Colour(0.5, alpha));
  }
  
  int radius = (width()>height()?height():width()) / 2;
  int center_x = width() / 2;
  int center_y = height() / 2;

  p.setColour(Colour(1, alpha));

  char buf[64];
  sprintf(buf, "%.2f", val * maximum);
  Font font;
  p.drawText(center_x - font.textWidth(buf) / 2,
             center_y + font.textHeight(buf) / 2, font, buf);

  p.setColour(Colour(0.5, 0.5, 1, alpha));

  p.drawCircle(center_x, center_y, radius);

  double padval = val * 0.8 + 0.1;
  double border_x = sin((-1 * padval + 1) * 2 * M_PI);
  double border_y = cos((-1 * padval + 1) * 2 * M_PI);

  p.setColour(Colour(1, 0, 0, alpha));
  p.drawLine(border_x * (radius / 2) + center_x,
             border_y * (radius / 2) + center_y,
             border_x * radius + center_x,
             border_y * radius + center_y);
}

#ifdef TEST_KNOB
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

#endif/*TEST_KNOB*/
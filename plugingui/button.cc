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

GUI::Button::Button(Widget *parent)
  : GUI::Widget(parent)
{
  state = up;
  handler = NULL;
  ptr = NULL;
}

void GUI::Button::registerClickHandler(void (*handler)(void *), void *ptr)
{
  this->handler = handler;
  this->ptr = ptr;
}

void GUI::Button::buttonEvent(ButtonEvent *e)
{
  if(e->direction == 1) {
    state = down;
    repaintEvent(NULL);
  }
  if(e->direction == -1) {
    state = up;
    repaintEvent(NULL);
    clicked();
    if(handler) handler(ptr);
  }
}

void GUI::Button::repaintEvent(GUI::RepaintEvent *e)
{
  //  printf("Button::repaintEvent\n");

  Painter p(this);

  float alpha = 0.8;

  if(hasKeyboardFocus()) {
    p.setColour(Colour(0.6, alpha));
  } else {
    p.setColour(Colour(0.5, alpha));
  }
  p.drawFilledRectangle(0,0,width()-1,height()-1);

  p.setColour(Colour(0.1, alpha));
  p.drawRectangle(0,0,width()-1,height()-1);

  p.setColour(Colour(0.8, alpha));
  switch(state) {
  case up:
    p.drawLine(0,0,0,height()-1);
    p.drawLine(0,0,width()-1,0);
    break;
  case down:
    p.drawLine(width()-1,0, width()-1,height()-1);
    p.drawLine(width()-1,height()-1,0, height()-1);
    break;
  }

  p.setColour(Colour(0.3, alpha));
  p.drawPoint(0,height()-1);
  p.drawPoint(width()-1,0);

  Font font;
  p.setColour(Colour(0.9, alpha));
  p.drawText(width()/2-(text.length()*3)+(state==up?0:1),
             height()/2+5+(state==up?0:1), font, text);
}

void GUI::Button::setText(std::string text)
{
  this->text = text;
  repaintEvent(NULL);
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

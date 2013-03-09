/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            checkbox.cc
 *
 *  Sat Nov 26 15:07:44 CET 2011
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
#include "checkbox.h"

#include "painter.h"

#include <stdio.h>

GUI::CheckBox::CheckBox(Widget *parent)
  : GUI::Widget(parent)
{
  state = false;
  handler = NULL;
}

void GUI::CheckBox::buttonEvent(ButtonEvent *e)
{
  if(e->direction == -1) {
    state = !state;
    repaintEvent(NULL);
    if(handler) handler(ptr);
  }
}

void GUI::CheckBox::setText(std::string text)
{
  _text = text;
  repaintEvent(NULL);
}

void GUI::CheckBox::registerClickHandler(void (*handler)(void *), void *ptr)
{
  this->handler = handler;
  this->ptr = ptr;
}

void GUI::CheckBox::repaintEvent(GUI::RepaintEvent *e)
{
  //  printf("CheckBox::repaintEvent\n");

  Painter p(this);

  float alpha = 0.8;

  int box = width()<height()?width():height();

  p.setColour(Colour(0.5, alpha));
  p.drawFilledRectangle(0,0,box-1,box-1);

  p.setColour(Colour(0.1, alpha));
  p.drawRectangle(0,0,box-1,box-1);

  if(state) {
    p.setColour(Colour(0.8, alpha));
    p.drawLine(0,0,box-1,box-1);
    p.drawLine(0,box-1,box-1,0);
  }

  p.setColour(Colour(0.3, alpha));
  p.drawPoint(0,box-1);
  p.drawPoint(box-1,0);

  p.setColour(Colour(1));
  Font font;
  p.drawText(box + 8, height() / 2 + 5, font, _text);
}

bool GUI::CheckBox::checked()
{
  return state;
}

void GUI::CheckBox::setChecked(bool c)
{
  state = c;
  repaintEvent(NULL);
}

#ifdef TEST_CHECKBOX
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

#endif/*TEST_CHECKBOX*/

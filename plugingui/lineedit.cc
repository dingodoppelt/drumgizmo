/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            lineedit.cc
 *
 *  Sun Oct  9 13:01:52 CEST 2011
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
#include "lineedit.h"

#include <stdio.h>

#include "painter.h"

#include "window.h"

#define BORDER 10

GUI::LineEdit::LineEdit(Widget *parent)
  : GUI::Widget(parent)
{
  pos = 0;
}

void GUI::LineEdit::setText(std::string text)
{
  _text = text;
  repaintEvent(NULL);
  textChanged();
}

std::string GUI::LineEdit::text()
{
  return _text;
}

void GUI::LineEdit::buttonEvent(ButtonEvent *e)
{
  if(e->direction == 1) {
    for(int i = 0; i < (int)_text.length(); i++) {
      if(e->x < (int)(font.textWidth(_text.substr(0, i)) + BORDER)) {
        pos = i;
        break;
      }
    }
    repaintEvent(NULL);
  }
}

void GUI::LineEdit::keyEvent(GUI::KeyEvent *e)
{
  bool change = false;
  
  if(e->direction == -1) {

    if(e->keycode == GUI::KeyEvent::KEY_LEFT) {
      if(pos) pos--;

    } else if(e->keycode == GUI::KeyEvent::KEY_HOME) {
      pos = 0;

    } else if(e->keycode == GUI::KeyEvent::KEY_END) {
      pos = _text.length();

    } else if(e->keycode == GUI::KeyEvent::KEY_RIGHT) {
      if(pos < _text.length()) pos++;

    } else if(e->keycode == GUI::KeyEvent::KEY_DELETE) {
      if(pos < _text.length()) {
        std::string t = _text.substr(0, pos);
        t += _text.substr(pos + 1, std::string::npos);
        _text = t;
        change = true;
      }

    } else if(e->keycode == GUI::KeyEvent::KEY_BACKSPACE) {
      if(pos > 0) {
        std::string t = _text.substr(0, pos - 1);
        t += _text.substr(pos, std::string::npos);
        _text = t;
        pos--;
        change = true;
      }

    } else if(e->keycode == GUI::KeyEvent::KEY_CHARACTER) {
      std::string pre = _text.substr(0, pos);
      std::string post = _text.substr(pos, std::string::npos);
      _text = pre + e->text + post;
      change = true;
      pos++;
    }

    repaintEvent(NULL);
  }

  if(change) textChanged();
}

void GUI::LineEdit::repaintEvent(GUI::RepaintEvent *e)
{
  Painter p(this);
  
  p.setColour(Colour(0, 0.4));
  p.drawFilledRectangle(3,3,width()-3,height()-3);

  p.setColour(Colour(1,1,1));
  p.drawRectangle(0,0,width()-1,height()-1);
  p.drawRectangle(2,2,width()-3,height()-3);
  p.drawText(BORDER, height()/2+5, font, _text);

  if(hasKeyboardFocus()) {
    size_t px = font.textWidth(_text.substr(0, pos));
    p.setColour(Colour(0.8));
    p.drawLine(px + BORDER - 1, 4, px + BORDER - 1, height() - 5);
  }
}

#ifdef TEST_LINEEDIT
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

#endif/*TEST_LINEEDIT*/

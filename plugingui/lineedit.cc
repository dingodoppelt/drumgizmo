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
#include "globalcontext.h"

#define BORDER 10

LineEdit::LineEdit(GlobalContext *gctx, Widget *parent)
  : Widget(gctx, parent)
{
  pos = 0;
  gctx->keyboardFocus = this;
}

void LineEdit::setText(std::string text)
{
  _text = text;
}

std::string LineEdit::text()
{
  return _text;
}

void LineEdit::key(KeyEvent *e)
{
  //  printf("%d\n", e->keycode);
  if(e->direction == -1) {

    if(e->keycode == 113) { // left key
      if(pos) pos--;

    } else if(e->keycode == 114) { // right key
      if(pos < _text.length()) pos++;

    } else if(e->keycode == 119) { // delete
      if(pos < _text.length()) {
        std::string t = _text.substr(0, pos);
        t += _text.substr(pos + 1, std::string::npos);
        _text = t;
      }

    } else if(e->keycode == 22) { // backspace
      if(pos > 0) {
        std::string t = _text.substr(0, pos - 1);
        t += _text.substr(pos, std::string::npos);
        _text = t;
        pos--;
      }

    } else if(e->keycode >= 10 && e->keycode <= 61) {
      _text += e->text;
      pos++;
    }

    repaint(NULL);
  }
}

void LineEdit::repaint(RepaintEvent *e)
{
  Painter p(gctx, wctx);
  
  p.setColour(Colour(0));
  p.drawFilledRectangle(3,3,width()-3,height()-3);

  p.setColour(Colour(1,1,1));
  p.drawRectangle(0,0,width()-1,height()-1);
  p.drawRectangle(2,2,width()-5,height()-5);
  p.drawText(BORDER, height()/2+4, _text);

  p.setColour(Colour(0.8));
  p.drawLine(pos * 6 + BORDER - 1, 4, pos * 6 + BORDER - 1, height() - 5);
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

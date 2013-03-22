/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            progressbar.cc
 *
 *  Fri Mar 22 22:07:57 CET 2013
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
#include "progressbar.h"

#include "painter.h"

GUI::ProgressBar::ProgressBar(GUI::Widget *parent) : GUI::Widget(parent)
{
  _progress = 0;
}

float GUI::ProgressBar::progress()
{
  return _progress;
}

void GUI::ProgressBar::setProgress(float progress)
{
  _progress = progress;
  repaintEvent(NULL);
}

void GUI::ProgressBar::repaintEvent(GUI::RepaintEvent *e)
{
  Painter p(this);

  int max = width() - 1;

  p.clear();

  p.setColour(Colour(0, 0, 1, 0.3));
  p.drawFilledRectangle(1, 1, max * _progress - 2, height() - 1);

  p.setColour(Colour(1));
  p.drawRectangle(0,0,max,height()-1);
}

#ifdef TEST_PROGRESSBAR
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

#endif/*TEST_PROGRESSBAR*/

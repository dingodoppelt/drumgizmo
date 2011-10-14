/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            colour.cc
 *
 *  Fri Oct 14 09:38:28 CEST 2011
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
#include "colour.h"

#include "globalcontext.h"

Colour::Colour()
{
  red = blue = green = alpha = 1.0;
}

Colour::Colour(float grey, float a)
{
  red = green = blue = grey;
  alpha = a;
}

Colour::Colour(float r, float g, float b, float a)
{
  red = r;
  green = g;
  blue = b;
  alpha = a;
}

#ifdef X11
int Colour::getColour(GlobalContext *gctx)
{
  XColor col;
  Colormap colormap = DefaultColormap(gctx->display, 0);
  //  XParseColor(gctx->display, colormap, "#00FF00", &col);
  col.red = 65535 * red;
  col.green = 65535 * green;
  col.blue = 65535 * blue;
  col.flags = DoRed | DoGreen | DoBlue;

  XAllocColor(gctx->display, colormap, &col);

  return col.pixel;
  /*
  int colour;

  if(red == 1.0 && green == 1.0 && blue == 1.0)
    colour = WhitePixel(gctx->display, DefaultScreen(gctx->display));
  else
    colour = BlackPixel(gctx->display, DefaultScreen(gctx->display));

  return colour;
  */
}
#endif/*X11*/

#ifdef TEST_COLOUR
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

#endif/*TEST_COLOUR*/

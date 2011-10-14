/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            painter.h
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
#ifndef __DRUMGIZMO_PAINTER_H__
#define __DRUMGIZMO_PAINTER_H__

#include <string>

#include "colour.h"

class GlobalContext;
class WidgetContext;

class Painter {
public:
  Painter(GlobalContext *gctx, WidgetContext *wctx);
  ~Painter();

  void flush();

  void setColour(Colour colour);

  void drawLine(int x1, int y1, int x2, int y2);
  void drawText(int x, int y, std::string text);
  void drawRectangle(int x1, int y1, int x2, int y2);
  void drawFilledRectangle(int x1, int y1, int x2, int y2);

private:
  GlobalContext *gctx;
  WidgetContext *wctx;
  Colour colour;
};


#endif/*__DRUMGIZMO_PAINTER_H__*/

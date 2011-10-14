/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            widget.h
 *
 *  Sun Oct  9 13:01:44 CEST 2011
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
#ifndef __DRUMGIZMO_WIDGET_H__
#define __DRUMGIZMO_WIDGET_H__

#include <vector>

class GlobalContext;
class WidgetContext;

#include "event.h"

class Widget {
public:
  Widget(GlobalContext *gctx, Widget *parent);
  ~Widget();

  void show();
  void hide();

  void setSize(size_t width, size_t height);
  void move(size_t x, size_t y);

  size_t x();
  size_t y();
  size_t width();
  size_t height();

  void addChild(Widget *widget);

  virtual void repaint(RepaintEvent *e) {}
  virtual void mouseMove(MouseMoveEvent *e) {}
  virtual void button(ButtonEvent *e) {}
  virtual void key(KeyEvent *e) {}
  
protected:
  GlobalContext *gctx;
  WidgetContext *wctx;
  std::vector<Widget*> children;
  Widget *parent;
  size_t _x, _y, _width, _height;
};

#endif/*__DRUMGIZMO_WIDGET_H__*/

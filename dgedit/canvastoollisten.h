/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            canvastoollisten.h
 *
 *  Fri Jul 29 16:57:48 CEST 2011
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
#ifndef __DRUMGIZMO_CANVASTOOLLISTEN_H__
#define __DRUMGIZMO_CANVASTOOLLISTEN_H__

#include "canvastool.h"

//#include <QTimer>

#include "canvas.h"
#include "player.h"

class CanvasToolListen : public CanvasTool {
Q_OBJECT
public:
  CanvasToolListen(Canvas *canvas, Player &player);

  QString name() { return "Listen"; }
  bool mousePressEvent(QMouseEvent *event);
  bool mouseReleaseEvent(QMouseEvent *event);
  void paintEvent(QPaintEvent *event, QPainter &painter);

  void playRange(unsigned int from, unsigned int to);
  
public slots:
  void update(size_t position);

private:
  Canvas *canvas;

  Player &player;

  size_t lastpos;
  size_t pos;
};

#endif/*__DRUMGIZMO_CANVASTOOLLISTEN_H__*/

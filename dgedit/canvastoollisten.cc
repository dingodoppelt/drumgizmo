/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            canvastoollisten.cc
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
#include "canvastoollisten.h"

#include <QApplication>

CanvasToolListen::CanvasToolListen(Canvas *c, Player &p)
  : player(p)
{
  lastpos = pos = 0;
  canvas = c ;
}

bool CanvasToolListen::mousePressEvent(QMouseEvent *event)
{
  if(!isActive()) return false;
  player.setPosition(canvas->unmapX(event->x()));
  canvas->update();
  connect(&player, SIGNAL(positionUpdate(size_t)), this, SLOT(update(size_t)));
  return true;
}

bool CanvasToolListen::mouseReleaseEvent(QMouseEvent *event)
{
  if(!isActive()) return false;
  player.stop();
  disconnect(&player, SIGNAL(positionUpdate(size_t)),
             this, SLOT(update(size_t)));
  lastpos = 0;
  canvas->update();
  return true;
}

void CanvasToolListen::paintEvent(QPaintEvent *event, QPainter &painter)
{
  if(!isActive()) return;

  if(player.pos < player.end) {
    painter.setPen(QColor(0, 127, 127));
    painter.drawLine(canvas->mapX(pos),
                     event->rect().y(),
                     canvas->mapX(pos),
                     event->rect().y() + event->rect().height());
   }
}

void CanvasToolListen::update(size_t pos)
{
  this->pos = pos;
  size_t last = canvas->mapX(lastpos);
  size_t x = canvas->mapX(player.pos);
  QRect r(last, 0,
          x - last + 2, canvas->height());
  
  canvas->update(r);
  lastpos = pos;
}

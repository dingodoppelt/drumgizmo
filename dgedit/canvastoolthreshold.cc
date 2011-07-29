/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            canvastoolthreshold.cc
 *
 *  Fri Jul 29 16:27:00 CEST 2011
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
#include "canvastoolthreshold.h"

#include <math.h>

#define mapX(x) canvas->mapX(x)
#define mapY(x) canvas->mapY(x)
#define unmapX(x) canvas->unmapX(x)
#define unmapY(x) canvas->unmapY(x)

CanvasToolThreshold::CanvasToolThreshold(Canvas *c)
{
  canvas = c;
  threshold = 0.5;
  threshold_is_moving = false;

  colThreshold = QColor(255, 127, 127);
  colThresholdMoving = QColor(180, 0, 0);
}

bool CanvasToolThreshold::mouseMoveEvent(QMouseEvent *event)
{
 if(event->button() != Qt::LeftButton) {
    if(abs(event->y() - mapY(threshold)) < 2 ||
       abs(event->y() - mapY(-threshold)) < 2 ) {
      canvas->setCursor(Qt::SplitVCursor);
      return true;
    }
  }

  if(threshold_is_moving) {
    float val = unmapY(event->y());
    if(fabs(val) > 1.0) val = 1.0;
    threshold = fabs(val);
    canvas->update();
    return true;
  }

  return false;
}

bool CanvasToolThreshold::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {

    // Check if threshold is being dragged.
    if(abs(event->y() - mapY(threshold)) < 2 ||
       abs(event->y() - mapY(-threshold)) < 2 ) {
      threshold_is_moving = true;
      canvas->update();
      return true;
    }
  }

  return false;
}

bool CanvasToolThreshold::mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    if(threshold_is_moving) {
      threshold_is_moving = false;
      canvas->setCursor(Qt::ArrowCursor);
      canvas->update();
      return true;
    }
  }

  return false;
}

void CanvasToolThreshold::paintEvent(QPaintEvent *event, QPainter &painter)
{
  if(threshold_is_moving) painter.setPen(colThresholdMoving);
  else painter.setPen(colThreshold);
  painter.drawLine(event->rect().x(), mapY(threshold),
                   event->rect().x() + event->rect().width(), mapY(threshold));
  painter.drawLine(event->rect().x(), mapY(-threshold),
                   event->rect().x() + event->rect().width(), mapY(-threshold));
}


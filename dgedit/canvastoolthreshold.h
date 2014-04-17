/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            canvastoolthreshold.h
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
#ifndef __DRUMGIZMO_CANVASTOOLTHRESHOLD_H__
#define __DRUMGIZMO_CANVASTOOLTHRESHOLD_H__

#include "canvastool.h"

#include <QColor>

#include "canvas.h"

class CanvasToolThreshold : public CanvasTool {
Q_OBJECT
public:
  CanvasToolThreshold(Canvas *canvas);

  QString name() { return "Threshold"; }
  bool mouseMoveEvent(QMouseEvent *event);
  bool mousePressEvent(QMouseEvent *event);
  bool mouseReleaseEvent(QMouseEvent *event);
  void paintEvent(QPaintEvent *event, QPainter &painter);

signals:
  void thresholdChanged(double threshold);
  void thresholdChanging(double threshold);

private:
  float threshold;
  bool threshold_is_moving;

  Canvas *canvas;

  QColor colThreshold;
  QColor colThresholdMoving;
};

#endif/*__DRUMGIZMO_CANVASTOOLTHRESHOLD_H__*/

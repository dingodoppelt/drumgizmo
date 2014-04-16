/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            canvastoolselections.h
 *
 *  Thu Jul 28 20:16:59 CEST 2011
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
#ifndef __DRUMGIZMO_CANVASTOOLSELECTIONS_H__
#define __DRUMGIZMO_CANVASTOOLSELECTIONS_H__

#include "canvastool.h"

#include <QColor>

#include "canvas.h"

#include "selection.h"

class CanvasToolSelections : public CanvasTool {
Q_OBJECT
public:
  CanvasToolSelections(Canvas *canvas, Selections &selections,
                       Selections &selections_preview);

  QString name() { return "Selections"; }
  bool mouseMoveEvent(QMouseEvent *event);
  bool mousePressEvent(QMouseEvent *event);
  bool mouseReleaseEvent(QMouseEvent *event);
  void paintEvent(QPaintEvent *event, QPainter &painter);
  void keyReleaseEvent(QKeyEvent *event);

  //Selections selections();

signals:
  //void selectionsChanged(Selections selections);
  //void activeSelectionChanged(sel_id_t id);

public slots:
  void autoCreateSelections();
  void autoCreateSelectionsPreview();
  void clearSelections();
  void thresholdChanged(double threshold);
  void noiseFloorChanged(int t);
  void fadeoutChanged(int t);
  void setShowPreview(bool show_preview);

private:
  void doAutoCreateSelections(bool preview);

  bool selection_is_moving_left;
  bool selection_is_moving_right;

  Canvas *canvas;

  double threshold;
  double noise_floor;
  double fadeout;
  
  QColor colSelBg;
  QColor colSel;
  QColor colActiveSelBg;
  QColor colActiveSel;
  QColor colPreviewSelBg;
  QColor colPreviewSel;

  Selections &selections;
  Selections &selections_preview;

  bool show_preview;
};

#endif/*__DRUMGIZMO_CANVASTOOLSELECTIONS_H__*/

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            canvastoolselections.cc
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
#include "canvastoolselections.h"

#include <math.h>
#include <stdio.h>

#define mapX(x) canvas->mapX(x)
#define mapY(x) canvas->mapY(x)
#define unmapX(x) canvas->unmapX(x)
#define unmapY(x) canvas->unmapY(x)

CanvasToolSelections::CanvasToolSelections(Canvas *c)
{
  threshold = 0.5; // Default from CanvasToolThreshold

  canvas = c;
  //  data = canvas->data;
  //  size = canvas->size;

  selection_is_moving_left = false;
  selection_is_moving_right = false;
  active_selection = NULL;

  colSelBg = QColor(255, 0, 0, 60);
  colSel = QColor(255, 0, 0, 160);
  colActiveSelBg = QColor(255, 255, 0, 60);
  colActiveSel = QColor(255, 255, 0, 160);

}

bool CanvasToolSelections::mouseMoveEvent(QMouseEvent *event)
{
  if(selection_is_moving_left) {
    float val = unmapX(event->x());
    if(val > active_selection->to) val = active_selection->to - 1;
    active_selection->from = val;
    canvas->update();
    emit selectionsChanged(_selections);
    return true;
  }

  if(selection_is_moving_right) {
    float val = unmapX(event->x());
    if(val < active_selection->from) val = active_selection->from + 1;
    active_selection->to = val;
    canvas->update();
    emit selectionsChanged(_selections);
    return true;
  }

  if(event->button() != Qt::LeftButton) {
    // Check if a selection is being dragged.
    QMap<int, Selection>::iterator i = _selections.begin();
    while(i != _selections.end()) {
      if(abs(event->x() - mapX(i.value().from)) < 2
        || abs(event->x() - mapX(i.value().to)) < 2) {
        canvas->setCursor(Qt::SplitHCursor);
        return true;
      }
      i++;
    }
  }

  return false;
}

bool CanvasToolSelections::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    // Check if a selection is being dragged.
    QMap<int, Selection>::iterator i = _selections.begin();
    while(i != _selections.end()) {
      if(abs(event->x() - mapX(i.value().from)) < 2) {
        active_selection = &i.value();
        selection_is_moving_left = true;
        emit activeSelectionChanged(i.value());
        return true;
      }

      if(abs(event->x() - mapX(i.value().to)) < 2) {
        active_selection = &i.value();
        selection_is_moving_right = true;
        emit activeSelectionChanged(i.value());
        return true;
      }

      i++;
    }

    // Check if a selection is being selected.
    i = _selections.begin();
    while(i != _selections.end()) {
      if(event->x() > mapX(i.value().from) &&
         event->x() < mapX(i.value().to)) {
        active_selection = &i.value();
        canvas->update();
        emit activeSelectionChanged(i.value());
        return true;
      }

      i++;
    }

    // Make new selection
    int from = unmapX(event->x());
    _selections[from] = Selection(from, from);
    active_selection = &_selections[from];
    selection_is_moving_right = true;
    canvas->update();
    return true;
  }

  return false;
}

bool CanvasToolSelections::mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    if(selection_is_moving_left || selection_is_moving_right) {
      selection_is_moving_left = false;
      selection_is_moving_right = false;
      canvas->setCursor(Qt::ArrowCursor);
      canvas->update();
      return true;
    }
  }

  return false;
}

void CanvasToolSelections::paintEvent(QPaintEvent *event, QPainter &painter)
{
  int pos = unmapX(event->rect().x());
  int width = unmapX(event->rect().width());
  QMap<int, Selection>::iterator i = _selections.begin();
  while(i != _selections.end()) {
    int from = i.value().from;
    int to = i.value().to;
    int fadein = i.value().fadein;
    int fadeout = i.value().fadeout;
    if(from > pos + width || to + width < pos) { i++; continue; }
    if(active_selection == &i.value()) {
      painter.setBrush(colActiveSelBg);
      painter.setPen(colActiveSel);
    } else {
      painter.setBrush(colSelBg);
      painter.setPen(colSel);
    }
    painter.drawRect(mapX(from), mapY(-1.0), mapX(to) - mapX(from), mapY(1.0) - mapY(-1.0));
    painter.drawLine(mapX(from), mapY(0.0), mapX(from + fadein), mapY(-1.0));
    painter.drawLine(mapX(from), mapY(0.0), mapX(from + fadein), mapY(1.0));
    painter.drawLine(mapX(to - fadeout), mapY(-1.0), mapX(to), mapY(0.0));
    painter.drawLine(mapX(to - fadeout), mapY(1.0), mapX(to), mapY(0.0));
    i++;
  }
}

void CanvasToolSelections::keyReleaseEvent(QKeyEvent *event)
{
  if(active_selection && event->key() == Qt::Key_Delete) {
    _selections.remove(active_selection->from);
    canvas->update();
  }
}

void CanvasToolSelections::thresholdChanged(double t)
{
  threshold = t;
}

void CanvasToolSelections::autoCreateSelections()
{
  float *data = canvas->data;
  size_t size = canvas->size;

  for(size_t i = 0; i < size; i++) {
    if(fabs(data[i]) > fabs(threshold)) {
      int from = i;

      if(data[from] > 0.0) {
        while(data[from] > data[from-1] // Falling
              && data[from-1] > 0.0 // Not crossing zero
              ) {
          from--;
        }
      } else if(data[from] < 0.0) {
        while(data[from] < data[from-1] // Rising
              && data[from-1] < 0.0 // Not crossing zero
              ) {
          from--;
        }
      }

      int minsize = 100; // attack.
      float minval = 0.00003; // noise floor
      int to = i;
      float runavg = fabs(data[from]);
      while((runavg > minval ||
             to < from + minsize) &&
            to < (int)size) {
        double p = 0.9;
        runavg = runavg * p + fabs(data[to]) * (1 - p);
        to++;
      }
      _selections[from] = Selection(from, to, 2, (to - from) / 3);

      i = to+1;
    }
  }
  canvas->update();
  emit selectionsChanged(_selections);
}

void CanvasToolSelections::clearSelections()
{
  _selections.clear();
  selection_is_moving_left = false;
  selection_is_moving_right = false;
  canvas->setCursor(Qt::ArrowCursor);
  canvas->update();
  emit selectionsChanged(_selections);
}

Selections CanvasToolSelections::selections()
{
  return _selections;
}

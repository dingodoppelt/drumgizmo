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

CanvasToolSelections::CanvasToolSelections(Canvas *c, Selections &s,
                                           Selections &p)
  : selections(s), selections_preview(p)
{
  threshold = 0.5; // Default from CanvasToolThreshold

  canvas = c;

  selection_is_moving_left = false;
  selection_is_moving_right = false;

  colSelBg = QColor(255, 0, 0, 60);
  colSel = QColor(255, 0, 0, 160);
  colActiveSelBg = QColor(255, 255, 0, 60);
  colActiveSel = QColor(255, 255, 0, 160);
  colPreviewSelBg = QColor(0, 0, 255, 60);
  colPreviewSel = QColor(0, 0, 255, 160);
}

bool CanvasToolSelections::mouseMoveEvent(QMouseEvent *event)
{
  sel_id_t active_selection = selections.active();
  Selection act_sel = selections.get(active_selection);

  if(selection_is_moving_left) {
    float val = unmapX(event->x());
    if(val > act_sel.to) val = act_sel.to - 1;
    act_sel.from = val;
    selections.update(active_selection, act_sel);
    canvas->update();
    return true;
  }

  if(selection_is_moving_right) {
    float val = unmapX(event->x());
    if(val < act_sel.from) val = act_sel.from + 1;
    act_sel.to = val;
    selections.update(active_selection, act_sel);
    canvas->update();
    return true;
  }

  if(event->button() != Qt::LeftButton) {
    // Check if a selection is being dragged.
    QVector<sel_id_t> ids = selections.ids();
    QVector<sel_id_t>::iterator i = ids.begin();
    while(i != ids.end()) {
      Selection sel = selections.get(*i);
      if(abs(event->x() - mapX(sel.from)) < 2
        || abs(event->x() - mapX(sel.to)) < 2) {
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
    QVector<sel_id_t> ids = selections.ids();
    QVector<sel_id_t>::iterator i = ids.begin();
    while(i != ids.end()) {
      Selection sel = selections.get(*i);
      if(abs(event->x() - mapX(sel.from)) < 2) {
        selections.setActive(*i);
        selection_is_moving_left = true;
        return true;
      }

      if(abs(event->x() - mapX(sel.to)) < 2) {
        selections.setActive(*i);
        selection_is_moving_right = true;
        return true;
      }

      i++;
    }

    // Check if a selection is being selected.
    i = ids.begin();
    while(i != ids.end()) {
      Selection sel = selections.get(*i);
      if(event->x() > mapX(sel.from) &&
         event->x() < mapX(sel.to)) {
        selections.setActive(*i);
        canvas->update();
        return true;
      }

      i++;
    }

    // Make new selection
    int from = unmapX(event->x());
    Selection new_selection(from, from);
    sel_id_t id = selections.add(new_selection);
    selections.setActive(id);
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

  {
    QVector<sel_id_t> ids = selections.ids();
    QVector<sel_id_t>::iterator i = ids.begin();
    while(i != ids.end()) {
      Selection sel = selections.get(*i);
      int from = sel.from;
      int to = sel.to;
      int fadein = sel.fadein;
      int fadeout = sel.fadeout;
      if(from > pos + width || to + width < pos) { i++; continue; }
      if(selections.active() == *i) {
        painter.setBrush(colActiveSelBg);
        painter.setPen(colActiveSel);
      } else {
        painter.setBrush(colSelBg);
        painter.setPen(colSel);
      }
      painter.drawRect(mapX(from), mapY(-1.0),
                       mapX(to) - mapX(from), mapY(1.0) - mapY(-1.0));
      painter.drawLine(mapX(from), mapY(0.0), mapX(from + fadein), mapY(-1.0));
      painter.drawLine(mapX(from), mapY(0.0), mapX(from + fadein), mapY(1.0));
      painter.drawLine(mapX(to - fadeout), mapY(-1.0), mapX(to), mapY(0.0));
      painter.drawLine(mapX(to - fadeout), mapY(1.0), mapX(to), mapY(0.0));
      i++;
    }
  }

  if(show_preview) {
    QVector<sel_id_t> ids = selections_preview.ids();
    QVector<sel_id_t>::iterator i = ids.begin();
    while(i != ids.end()) {
      Selection sel = selections_preview.get(*i);
      int from = sel.from;
      int to = sel.to;
      int fadein = sel.fadein;
      int fadeout = sel.fadeout;
      if(from > pos + width || to + width < pos) { i++; continue; }
      painter.setBrush(colPreviewSelBg);
      painter.setPen(colPreviewSel);
      
      painter.drawRect(mapX(from), mapY(-1.0),
                       mapX(to) - mapX(from), mapY(1.0) - mapY(-1.0));
      painter.drawLine(mapX(from), mapY(0.0), mapX(from + fadein), mapY(-1.0));
      painter.drawLine(mapX(from), mapY(0.0), mapX(from + fadein), mapY(1.0));
      painter.drawLine(mapX(to - fadeout), mapY(-1.0), mapX(to), mapY(0.0));
      painter.drawLine(mapX(to - fadeout), mapY(1.0), mapX(to), mapY(0.0));
      i++;
    }
  }
}

void CanvasToolSelections::keyReleaseEvent(QKeyEvent *event)
{
  if(selections.active() != SEL_NONE && event->key() == Qt::Key_Delete) {
    selections.remove(selections.active());
    canvas->update();
  }
}

void CanvasToolSelections::thresholdChanged(double t)
{
  threshold = t;
}

void CanvasToolSelections::noiseFloorChanged(int t)
{
  double div = 666.0 / 0.00003;
  noise_floor = (double)t/div;
}

void CanvasToolSelections::fadeoutChanged(int t)
{
  fadeout = (double)t/1000.0;
}

void CanvasToolSelections::autoCreateSelections()
{
  doAutoCreateSelections(false);
}

void CanvasToolSelections::autoCreateSelectionsPreview()
{
  doAutoCreateSelections(true);
}

void CanvasToolSelections::doAutoCreateSelections(bool preview)
{
  float *data = canvas->data;
  size_t size = canvas->size;

  if(preview) selections_preview.clear();

  for(size_t i = 0; i < size; i++) {
    if(fabs(data[i]) > fabs(threshold)) {
      int from = i;

      // FIXME: This doesn't work if the recording has a DC offset.
      if(data[from] > 0.0) {
        while(data[from] > 0.0) { // Not crossing zero
          from--;
        }
        from++;
      } else if(data[from] < 0.0) {
        while(data[from-1] < 0.0) { // Not crossing zero
          from--;
        }
        from++;
      }

      int minsize = 100; // attack.
      int to = i;
      float runavg = fabs(data[from]);
      while((runavg > noise_floor ||
             to < from + minsize) &&
            to < (int)size) {
        double p = 0.9;
        runavg = runavg * p + fabs(data[to]) * (1 - p);
        to++;
      }

      Selection s(from, to, 2, ((to - from) / 3) * fadeout);
      if(preview) {
        selections_preview.add(s);
      } else {
        selections.add(s);
      }

      i = to+1;
    }
  }
  canvas->update();
}

void CanvasToolSelections::clearSelections()
{
  selections.clear();
  selection_is_moving_left = false;
  selection_is_moving_right = false;
  canvas->setCursor(Qt::ArrowCursor);
  canvas->update();
}

void CanvasToolSelections::setShowPreview(bool s)
{
  show_preview = s;
  canvas->update();
}

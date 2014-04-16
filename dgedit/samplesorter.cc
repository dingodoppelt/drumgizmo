/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            samplesorter.cc
 *
 *  Mon Nov 30 07:45:58 CET 2009
 *  Copyright 2009 Bent Bisballe Nyeng
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
#include "samplesorter.h"

#include <QPainter>
#include <QPaintEvent>

#include <stdio.h>

#include <math.h>
#ifndef MAXFLOAT
#define MAXFLOAT (3.40282347e+38F)
#endif

SampleSorter::SampleSorter(Selections &s, Selections &p)
  : selections(s), selections_preview(p)
{
  setMouseTracking(true);

  data = NULL;
  size = 0;
  attlen = 666; // Magical constants needs biblical proportions...

  sel_moving = SEL_NONE;
}

void SampleSorter::setShowPreview(bool s)
{
  show_preview = s;
  update();
}

void SampleSorter::setWavData(const float *data, size_t size)
{
  this->data = data;
  this->size = size;
  relayout();
}

int SampleSorter::attackLength()
{
  return attlen;
}

void SampleSorter::setAttackLength(int len)
{
  attlen = len;
  relayout();
}

void SampleSorter::addSelection(sel_id_t id)
{
  Selection s = selections.get(id);

  double energy = 0.0;
  for(size_t idx = s.from;
      (idx < (size_t)s.from + (size_t)attackLength()) &&
        (idx < (size_t)s.to) && (idx < size);
      idx++) {
    energy += data[idx] * data[idx];
  }
    
  s.energy = energy;
  selections.update(id, s);

  relayout();
}

void SampleSorter::addSelectionPreview(sel_id_t id)
{
  Selection s = selections_preview.get(id);

  double energy = 0.0;
  for(size_t idx = s.from;
      (idx < (size_t)s.from + (size_t)attackLength()) &&
        (idx < (size_t)s.to) && (idx < size);
      idx++) {
    energy += data[idx] * data[idx];
  }
    
  s.energy = energy;
  selections_preview.update(id, s);

  relayout();
}

void SampleSorter::relayout()
{
  min = MAXFLOAT;
  max = 0.0;

  {
    QVector<sel_id_t> ids = selections.ids();
    QVector<sel_id_t>::iterator i = ids.begin();
    while(i != ids.end()) {
      Selection sel = selections.get(*i);
      
      if(sel.energy < min) min = sel.energy;
      if(sel.energy > max) max = sel.energy;
      
      i++;
    }
  }

  if(show_preview) {
    QVector<sel_id_t> ids = selections_preview.ids();
    QVector<sel_id_t>::iterator i = ids.begin();
    while(i != ids.end()) {
      Selection sel = selections_preview.get(*i);
      
      if(sel.energy < min) min = sel.energy;
      if(sel.energy > max) max = sel.energy;
      
      i++;
    }
  }

  update();
}

#define MAP(p) (height()-(int)(p*((float)height()/(float)width())))

#define unmapX(x) ((double)x/(double)(width()-1))
#define unmapY(x) x
#define mapX(x) (((double)x)*(width()-1))
#define mapY(x) x

#define C_RADIUS 2
static void drawCircle(QPainter &p, int x, int y)
{
  p.drawEllipse(x - C_RADIUS, y - C_RADIUS, 2 * C_RADIUS, 2 * C_RADIUS);
}

void SampleSorter::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);

  QColor colBg = QColor(180, 200, 180);
  QColor colFg = QColor(160, 180, 160);
  QColor colPt = QColor(255, 100, 100);
  QColor colPtPreview = QColor(0, 0, 255);
  QColor colPtSel = QColor(255, 255, 100);

  painter.setPen(colBg);
  painter.setBrush(colBg);
  painter.drawRect(event->rect());

  painter.setPen(colFg);
  painter.drawLine(0,height(),width(),0);
  
  {
    QVector<sel_id_t> ids = selections.ids();
    QVector<sel_id_t>::iterator i = ids.begin();
    while(i != ids.end()) {
      Selection sel = selections.get(*i);
      if(*i == selections.active()) painter.setPen(colPtSel);
      else painter.setPen(colPt);
      float x = sel.energy / max;
      x = sqrt(x);
      x *= (float)width();
      drawCircle(painter, x, MAP(x));
      i++;
    }
  }

  if(show_preview) {
    QVector<sel_id_t> ids = selections_preview.ids();
    QVector<sel_id_t>::iterator i = ids.begin();
    while(i != ids.end()) {
      Selection sel = selections_preview.get(*i);
      painter.setPen(colPtPreview);
      float x = sel.energy / max;
      x = sqrt(x);
      x *= (float)width();
      drawCircle(painter, x, MAP(x));
      i++;
    }
  }


}

sel_id_t SampleSorter::getSelectionByCoordinate(int px, int py)
{
  // Hit radius is slithly larger than the circles themselves.
  int hit_r = C_RADIUS + 1;

  QVector<sel_id_t> ids = selections.ids();
  QVector<sel_id_t>::iterator i = ids.begin();
  while(i != ids.end()) {
    Selection sel = selections.get(*i);
    float x = (sel.energy/max);
    x = sqrt(x);
    x *= (float)width();
    if(px < (x + hit_r) && px > (x - hit_r) &&
       py < (MAP(x) + hit_r) && py > (MAP(x) - hit_r)) {
      return *i;
    }
    i++;
  }

  return SEL_NONE;
}

void SampleSorter::mouseMoveEvent(QMouseEvent *event)
{
  if(sel_moving != SEL_NONE) {
    Selection sel = selections.get(sel_moving);
    if(sel_moving != SEL_NONE) {
      double power = unmapX(event->x());
      power *= power;
      power *= max;
      sel.energy = power;
      selections.update(sel_moving, sel);
    }

    update();
    return;
  } else {
    sel_id_t psel = getSelectionByCoordinate(event->x(), event->y());
    if(psel != SEL_NONE) {
      setCursor(Qt::UpArrowCursor);
    } else {
      setCursor(Qt::ArrowCursor);
    }
  }
}

void SampleSorter::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    sel_id_t psel = getSelectionByCoordinate(event->x(), event->y());
    sel_moving = psel;
    selections.setActive(psel);
    if(psel != SEL_NONE) {
      setCursor(Qt::UpArrowCursor);
    }
  }
}

void SampleSorter::mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    sel_moving = SEL_NONE;
    setCursor(Qt::ArrowCursor);
  }
}

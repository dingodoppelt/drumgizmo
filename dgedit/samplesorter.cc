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

SampleSorter::SampleSorter()
{
  setMouseTracking(true);

  data = NULL;
  size = 0;
  attlen = 666; // Magical constants needs biblical proportions...

  cur_thr = -1;
  threshold.push_back(0.8);
  threshold_is_moving = false;
}

void SampleSorter::setWavData(const float *data, size_t size)
{
  this->data = data;
  this->size = size;
  resort();
}

void SampleSorter::setSelections(Selections s)
{
  _selections = s;
  resort();
}

int SampleSorter::attackLength()
{
  return attlen;
}

void SampleSorter::setAttackLength(int len)
{
  attlen = len;
  resort();
}

Selections SampleSorter::selections()
{
  Selections s;
  int j = 0;

  QMap<float, Selection>::iterator i = sorted.begin();
  while(i != sorted.end()) {
    s[j++] = i.value();
    i++;
  }

  return s;
}

Levels SampleSorter::levels()
{
  Levels lvls;

  // Sort the segmentation lines:
  for(int i = 0; i < threshold.size(); i++) {
    for(int j = 0; j < threshold.size(); j++) {
      if(threshold[i] < threshold[j]) {
        float tmp = threshold[i];
        threshold[i] = threshold[j];
        threshold[j] = tmp;
      }
    }
  }
  
  // 
  for(int i = -1; i < threshold.size(); i++) {
    Level lvl;
    
    if(i == -1) lvl.velocity = 0;
    else lvl.velocity = threshold[i] * threshold[i];
    
    float next;
    if(i == threshold.size() - 1) next = 1.0;
    else next = threshold[i+1] * threshold[i+1];

    QMap<float, Selection>::iterator si = sorted.begin();
    while(si != sorted.end()) {
      float val = (si.key()/max);
      if(val >= lvl.velocity && val <= next) {
        lvl.selections[si.key()] = si.value();
      }
      si++;
    }

    lvls.push_back(lvl);
  }

  return lvls;
}


void SampleSorter::resort()
{
  sorted.clear();

  min = MAXFLOAT;
  max = 0.0;

  QMap<int, Selection>::iterator i = _selections.begin();
  while(i != _selections.end()) {
    float energy = 0.0;
    Selection &s = i.value();

    for(size_t idx = s.from;
        (idx < (size_t)s.from + (size_t)attackLength()) &&
          (idx < (size_t)s.to) && (idx < size);
        idx++) {
      energy += data[idx] * data[idx];
    }
    
    while(sorted.find(energy) != sorted.end()) {
      energy += 1; // Make sure that the key is unique.
    }

    s.energy = energy;

    sorted[energy] = i.value();
    
    if(energy < min) min = energy;
    if(energy > max) max = energy;

    i++;
  }

  update();
}

void SampleSorter::setActiveSelection(Selection s)
{
  sel = s;
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
  QColor colPtSel = QColor(255, 255, 100);
  QColor colVel = QColor(0, 0, 0);

  painter.setPen(colBg);
  painter.setBrush(colBg);
  painter.drawRect(event->rect());

  painter.setPen(colFg);
  painter.drawLine(0,height(),width(),0);

  for(int i = 0; i < threshold.size(); i++) {
    if(cur_thr == i) painter.setPen(colPtSel);
    else painter.setPen(colPt);
    painter.drawLine(mapX(threshold[i]), 0, mapX(threshold[i]), height());
    char valstr[32];
    sprintf(valstr, "%.3f", threshold[i] * threshold[i]);
    painter.setPen(colVel);
    painter.drawText(mapX(threshold[i]), height(), valstr);
  }

  if(sorted.isEmpty()) return;

  QMap<float, Selection>::iterator i = sorted.begin();
  while(i != sorted.end()) {
    if(sel.to == i.value().to && sel.from == i.value().from)
      painter.setPen(colPtSel);
    else painter.setPen(colPt);
    float x = (i.key()/max);
    x = sqrt(x);
    x *= (float)width();
    drawCircle(painter, x, MAP(x));
    i++;
  }
}

Selection *SampleSorter::getSelectionByCoordinate(int px, int py)
{
  py -= C_RADIUS;

  QMap<float, Selection>::iterator i = sorted.begin();
  while(i != sorted.end()) {
    float x = (i.key()/max);
    x = sqrt(x);
    x *= (float)width();
    if(px < (x + C_RADIUS) && px > (x - C_RADIUS) &&
       py < (MAP(x) + C_RADIUS) && py > (MAP(x) - C_RADIUS)) {
      return &i.value();
    }
    i++;
  }

  return NULL;
}


void SampleSorter::mouseMoveEvent(QMouseEvent *event)
{
  if(cur_thr != -1 && cur_thr < threshold.size()) {
    float val = unmapX(event->x());
    if(val < 0.0) val = 0.0;
    if(val > 1.0) val = 1.0;
    threshold[cur_thr] = fabs(val);
    update();
    return;
  }

  if(event->button() != Qt::LeftButton) {
    setCursor(Qt::ArrowCursor);
    for(size_t i = 0; i < (size_t)threshold.size(); i++) {
      if(abs(event->x() - mapX(threshold[i])) < 2 ||
         abs(event->x() - mapX(-threshold[i])) < 2 ) {
        setCursor(Qt::SplitHCursor);
      }
    }
  }
}

void SampleSorter::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {

    Selection *psel = getSelectionByCoordinate(event->x(), event->y());
    if(psel) {
      emit activeSelectionChanged(*psel);
      return;
    }

    // Check if threshold is being dragged.
    for(size_t i = 0; i < (size_t)threshold.size(); i++) {
      if(abs(event->x() - mapX(threshold[i])) < 2 ||
         abs(event->x() - mapX(-threshold[i])) < 2 ) {
        cur_thr = i;
        threshold_is_moving = true;
        update();
        return;
      }
    }

    // Make new selection
    float from = unmapX(event->x());
    threshold.push_back(from);
    cur_thr = threshold.size() - 1;
    threshold_is_moving = true;
    update();
    return;
  }
}

void SampleSorter::mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    if(threshold_is_moving) {
      if(threshold[cur_thr] == 0.0 || threshold[cur_thr] == 1.0) {
        threshold.remove(cur_thr);
      }
      threshold_is_moving = false;
      cur_thr = -1;
      setCursor(Qt::ArrowCursor);
      update();
      return;
    }
  }
}

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

#include <math.h>
#ifndef MAXFLOAT
#define MAXFLOAT (3.40282347e+38F)
#endif

SampleSorter::SampleSorter()
{
  data = NULL;
  size = 0;
  attlen = 666; // Magical constants needs biblical proportions...
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

void SampleSorter::resort()
{
  sorted.clear();

  min = MAXFLOAT;
  max = 0.0;

  QMap<int, Selection>::iterator i = _selections.begin();
  while(i != _selections.end()) {
    float energy = 0.0;
    Selection s = i.value();

    for(size_t idx = s.from;
        (idx < (size_t)s.from + (size_t)attackLength()) && (idx < (size_t)s.to) && (idx < size);
        idx++) {
      energy += data[idx] * data[idx];
    }
    
    while(sorted.find(energy) != sorted.end()) {
      energy += 1; // Make sure that the key is unique.
    }

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

static void drawCircle(QPainter &p, int x, int y)
{
  p.drawEllipse(x-2, y-2, 4, 4);
}

void SampleSorter::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);

  QColor colBg = QColor(180, 200, 180);
  QColor colFg = QColor(160, 180, 160);
  QColor colPt = QColor(255, 100, 100);
  QColor colPtSel = QColor(255, 255, 100);

  painter.setPen(colBg);
  painter.setBrush(colBg);
  painter.drawRect(event->rect());

  painter.setPen(colFg);
  painter.drawLine(0,height(),width(),0);

  if(sorted.isEmpty()) return;

  QMap<float, Selection>::iterator i = sorted.begin();
  while(i != sorted.end()) {
    if(sel.to == i.value().to && sel.from == i.value().from) painter.setPen(colPtSel);
    else painter.setPen(colPt);
    float x = (i.key()/max)*(float)width();
    drawCircle(painter, x, MAP(x));
    i++;
  }
}

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            canvas.cc
 *
 *  Tue Nov 10 08:37:37 CET 2009
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
#include "canvas.h"

#include <sndfile.h>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QKeyEvent>

#include <math.h>

#define DEFYSCALE 200

Canvas::Canvas(QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_StaticContents);
  setMouseTracking(true);
  setFocusPolicy(Qt::ClickFocus);

  mipmap = NULL;

  data = NULL;
  size = 0;

  xscale = 1.0;
  yscale = 1.0;
  xoffset = 0.0;
  yoffset = 0.5;

  threshold = 0.5;
  threshold_is_moving = false;
  selection_is_moving_left = false;
  selection_is_moving_right = false;
  active_selection = NULL;

  colBg = QColor(180, 200, 180);
  colSec = QColor(160, 180, 160);
  colWavMax = QColor(100, 100, 100);
  colWavAvg = QColor(0, 0, 0);
  colMax = QColor(127, 127, 255);
  colHalf = QColor(180, 180, 255);
  colThreshold = QColor(255, 127, 127);
  colThresholdMoving = QColor(180, 0, 0);
  colSelBg = QColor(255, 0, 0, 60);
  colSel = QColor(255, 0, 0, 160);
  colActiveSelBg = QColor(255, 255, 0, 60);
  colActiveSel = QColor(255, 255, 0, 160);

  setCursor(Qt::ArrowCursor);
  
  wav = QImage(width(), height(), QImage::Format_RGB32);
}

Canvas::~Canvas()
{
  if(data) delete[] data;
  if(mipmap) delete mipmap;
}

void Canvas::load(QString file)
{
  if(data) {
    delete[] data;
    data = NULL;
    size = 0;
  }

  if(mipmap) {
    delete mipmap;
    mipmap = NULL;
  }

  SF_INFO sf_info;
	SNDFILE *fh = sf_open(file.toStdString().c_str(), SFM_READ, &sf_info);
  if(!fh) {
    printf("Load error...\n");
    return;
  }

  size = sf_info.frames;

  printf("Size: %u\n", (unsigned int)sf_info.frames);
  data = new float[size];

	sf_read_float(fh, data, size); 

	sf_close(fh);

  mipmap = new MipMap(data, size);

  updateWav();
  update();
}


#define SCALEX ((xscale * (float)size/(float)width()) + 0.1)
#define OFFSETX (xoffset * (float)size)
float Canvas::mapX(float x)
{
  float val = (x - OFFSETX) / SCALEX;
  return val;
}

float Canvas::unmapX(float x)
{
  float val = x * SCALEX + OFFSETX;
  return val;
}

#define SCALEY ((yscale * 999.0 + 1.0 ) * (float)DEFYSCALE)
#define OFFSETY (((float)height() / 2.0) + ((yoffset * 2.0 - 1.0) * SCALEY))
float Canvas::mapY(float y)
{
  float val = OFFSETY + (y * SCALEY);
  return val;
}

float Canvas::unmapY(float y)
{
  float val = (y - OFFSETY) / SCALEY;
  return val;
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
  if(threshold_is_moving) {
    float val = unmapY(event->y());
    if(fabs(val) > 1.0) val = 1.0;
    threshold = fabs(val);
    update();
    return;
  }

  if(selection_is_moving_left) {
    float val = unmapX(event->x());
    if(val > active_selection->to) val = active_selection->to - 1;
    active_selection->from = val;
    update();
    emit selectionsChanged(_selections);
    return;
  }

  if(selection_is_moving_right) {
    float val = unmapX(event->x());
    if(val < active_selection->from) val = active_selection->from + 1;
    active_selection->to = val;
    update();
    emit selectionsChanged(_selections);
    return;
  }

  if(event->button() != Qt::LeftButton) {
    if(abs(event->y() - mapY(threshold)) < 2 ||
       abs(event->y() - mapY(-threshold)) < 2 ) {
      setCursor(Qt::SplitVCursor);
    } else {
      setCursor(Qt::ArrowCursor);
    }

    // Check if a selection is being dragged.
    QMap<int, Selection>::iterator i = _selections.begin();
    while(i != _selections.end()) {
      if(abs(event->x() - mapX(i.value().from)) < 2
        || abs(event->x() - mapX(i.value().to)) < 2) {
        setCursor(Qt::SplitHCursor);
      }
      i++;
    }
  }
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {

    // Check if threshold is being dragged.
    if(abs(event->y() - mapY(threshold)) < 2 ||
       abs(event->y() - mapY(-threshold)) < 2 ) {
      threshold_is_moving = true;
      update();
      return;
    }

    // Check if a selection is being dragged.
    QMap<int, Selection>::iterator i = _selections.begin();
    while(i != _selections.end()) {
      if(abs(event->x() - mapX(i.value().from)) < 2) {
        active_selection = &i.value();
        selection_is_moving_left = true;
        emit activeSelectionChanged(i.value());
        return;
      }

      if(abs(event->x() - mapX(i.value().to)) < 2) {
        active_selection = &i.value();
        selection_is_moving_right = true;
        emit activeSelectionChanged(i.value());
        return;
      }

      i++;
    }

    // Check if a selection is being selected.
    i = _selections.begin();
    while(i != _selections.end()) {
      if(event->x() > mapX(i.value().from) &&
         event->x() < mapX(i.value().to)) {
        active_selection = &i.value();
        update();
        emit activeSelectionChanged(i.value());
        return;
      }

      i++;
    }

    // Make new selection
    int from = unmapX(event->x());
    _selections[from] = Selection(from, from);
    active_selection = &_selections[from];
    selection_is_moving_right = true;
    update();
    return;
  }
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    if(threshold_is_moving) {
      threshold_is_moving = false;
      setCursor(Qt::ArrowCursor);
      update();
      return;
    }
    if(selection_is_moving_left || selection_is_moving_right) {
      selection_is_moving_left = false;
      selection_is_moving_right = false;
      setCursor(Qt::ArrowCursor);
      update();
      return;
    }
  }
}

void Canvas::resizeEvent(QResizeEvent *)
{
  wav = QImage(width(), height(), QImage::Format_RGB32);
  updateWav();
  update();
}

void Canvas::getWavValues(int last, int lx, float *vu, float *vl,
                          float *avgu, float *avgl)
{
  if(mipmap == NULL) return;

  MipMapValue val = mipmap->lookup(last, lx);

  *vu = val.max;
  *vl = val.min;
  *avgu = val.uavg;
  *avgl = val.lavg;
}

void Canvas::updateWav()
{
  QPainter painter(&wav);

  painter.setPen(colBg);
  painter.setBrush(colBg);
  painter.drawRect(0, 0, wav.width(), wav.height());

  painter.setPen(colSec);
  int step = 44100;
  for(size_t i = 0; i < size; i += step) {
    painter.drawLine(mapX(i), mapY(1.0), mapX(i), mapY(-1.0));
  }

  painter.setPen(colMax);
  painter.drawLine(0, mapY(1.0), wav.width(), mapY(1.0));
  painter.drawLine(0, mapY(-1.0), wav.width(), mapY(-1.0));

  painter.setPen(colHalf);
  painter.drawLine(0, mapY(0.5), wav.width(), mapY(0.5));
  painter.drawLine(0, mapY(-0.5), wav.width(), mapY(-0.5));

  if(data) {
    int last = unmapX(0);
    for(int x = 0; x < wav.width(); x++) {
      int lx = unmapX(x);
      if(lx > (int)size || lx < 0) break;
      float vu = 0;
      float vl = 0;
      float avgu = 0;
      float avgl = 0;

      getWavValues(last, lx, &vu, &vl, &avgu, &avgl);
      
      int c = mapY(0.0);

      painter.setPen(colWavMax);
      painter.drawLine(x, c, x, mapY(vu));
      painter.drawLine(x, c, x, mapY(vl));

      painter.setPen(colWavAvg);
      painter.drawLine(x, c, x, mapY(avgu));
      painter.drawLine(x, c, x, mapY(avgl));

      last = lx;
    }
  }
}

void Canvas::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);

  painter.drawImage(event->rect(),wav,event->rect());

  if(threshold_is_moving) painter.setPen(colThresholdMoving);
  else painter.setPen(colThreshold);
  painter.drawLine(event->rect().x(), mapY(threshold),
                   event->rect().x() + event->rect().width(), mapY(threshold));
  painter.drawLine(event->rect().x(), mapY(-threshold),
                   event->rect().x() + event->rect().width(), mapY(-threshold));

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

void Canvas::keyReleaseEvent(QKeyEvent *event)
{
  if(active_selection && event->key() == Qt::Key_Delete) {
    _selections.remove(active_selection->from);
    update();
  }
}

void Canvas::setXScale(float scale)
{
  scale = (pow(100.0,scale) / 100.0) - (pow(100.0, 0.0)/ 100.0);
  if(scale < 0.0) scale = 0.0;
  if(scale > 1.0) scale = 1.0;
  xscale = scale;
  updateWav();
  update();
}

void Canvas::setYScale(float scale)
{ 
  scale = (pow(100.0,scale) / 100.0) - (pow(100.0, 0.0)/ 100.0);
  if(scale < 0.0) scale = 0.0;
  if(scale > 1.0) scale = 1.0;
  yscale = scale;
  updateWav();
  update();
}

void Canvas::setXOffset(float offset)
{
  if(offset < 0.0) offset = 0.0;
  if(offset > 1.0) offset = 1.0;
  xoffset = offset;
  updateWav();
  update();
}

void Canvas::setYOffset(float offset)
{
  if(offset < 0.0) offset = 0.0;
  if(offset > 1.0) offset = 1.0;
  yoffset = offset;
  updateWav();
  update();
}

void Canvas::autoCreateSelections()
{
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
  update();
  emit selectionsChanged(_selections);
}

void Canvas::clearSelections()
{
  _selections.clear();
  selection_is_moving_left = false;
  selection_is_moving_right = false;
  setCursor(Qt::ArrowCursor);
  update();
  emit selectionsChanged(_selections);
}

Selections Canvas::selections()
{
  return _selections;
}

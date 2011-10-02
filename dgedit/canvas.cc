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

  colBg = QColor(180, 200, 180);
  colSec = QColor(160, 180, 160);
  colWavMax = QColor(100, 100, 100);
  colWavAvg = QColor(0, 0, 0);
  colMax = QColor(127, 127, 255);
  colHalf = QColor(180, 180, 255);

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
  for(int i = 0; i < tools.size(); i++) {
    if(tools[i]->mouseMoveEvent(event)) return;
  }

  setCursor(Qt::ArrowCursor);
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
  for(int i = 0; i < tools.size(); i++) {
    if(tools[i]->mousePressEvent(event)) return;
  }
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
  for(int i = 0; i < tools.size(); i++) {
    if(tools[i]->mouseReleaseEvent(event)) return;
  }
}

void Canvas::resizeEvent(QResizeEvent *event)
{
  for(int i = 0; i < tools.size(); i++) {
    tools[i]->resizeEvent(event);
  }

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

  for(int i = 0; i < tools.size(); i++) {
    tools[i]->paintEvent(event, painter);
  }
} 

void Canvas::keyReleaseEvent(QKeyEvent *event)
{
  for(int i = 0; i < tools.size(); i++) {
    tools[i]->keyReleaseEvent(event);
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

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            canvas.h
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
#ifndef __DRUMGIZMO_CANVAS_H__
#define __DRUMGIZMO_CANVAS_H__

#include <QWidget>
#include <QColor>
#include <QImage>

#include "mipmap.h"
#include "canvastool.h"

class Canvas : public QWidget {
Q_OBJECT
public:
  Canvas(QWidget *parent);
  ~Canvas();

  void load(QString file);
  
  void addTool(CanvasTool *tool);

  float mapX(float x);
  float unmapX(float x);
  float mapY(float y);
  float unmapY(float y);

public slots:
  void setXScale(float scale);
  void setYScale(float scale);
  void setXOffset(float scroll);
  void setYOffset(float scroll);

protected:
  void mouseMoveEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void resizeEvent(QResizeEvent *event);
  void paintEvent(QPaintEvent *event);
  void keyReleaseEvent(QKeyEvent *event);

private:
  MipMap *mipmap;

  void updateWav();
  void getWavValues(int last, int lx, float *vu, float *vl,
                    float *avgu, float *avgl);

  QImage wav;

public:
  float *data;
  size_t size;
private:
  float xscale;
  float yscale;
  float xoffset;
  float yoffset;

  QColor colBg;
  QColor colSec;
  QColor colMax;
  QColor colHalf;
  QColor colWavMax;
  QColor colWavAvg;

  QVector<CanvasTool*> tools;
};

#endif/*__DRUMGIZMO_CANVAS_H__*/

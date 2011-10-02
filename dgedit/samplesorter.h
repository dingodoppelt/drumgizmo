/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            samplesorter.h
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
#ifndef __DRUMGIZMO_SAMPLESORTER_H__
#define __DRUMGIZMO_SAMPLESORTER_H__

#include <QWidget>
#include <QVector>
#include "selection.h"

class Level {
public:
  float velocity;
  QMap<float, Selection> selections;
};

typedef QVector<Level> Levels;

class SampleSorter : public QWidget {
Q_OBJECT
public:
  SampleSorter();

  Selections selections();
  Levels levels();

public slots:
  void setSelections(Selections selections);
  void setWavData(const float *data, size_t size);
  void resort();
  void setAttackLength(int len);
  int attackLength();
  void setActiveSelection(Selection s);

protected:
  void paintEvent(QPaintEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

private:
  Selections _selections;
  QMap<float, Selection> sorted;
  float min;
  float max;
  int attlen;

  // Wav data
  const float *data;
  size_t size;

  Selection sel;

  QVector<double> threshold;
  bool threshold_is_moving;
  bool selection_is_moving_left;
  bool selection_is_moving_right;
  int cur_thr;

};

#endif/*__DRUMGIZMO_SAMPLESORTER_H__*/

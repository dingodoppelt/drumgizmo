/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            player.h
 *
 *  Tue Apr 15 10:34:18 CEST 2014
 *  Copyright 2014 Bent Bisballe Nyeng
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
#ifndef __DRUMGIZMO_PLAYER_H__
#define __DRUMGIZMO_PLAYER_H__

#include <QThread>
#include <QTimer>

#include <ao/ao.h>

#include "canvas.h"

class Player : public QThread {
Q_OBJECT
public:
  Player(Canvas *canvas); // TODO: Don't use canvas pointer here. Instead introduce setData slot or similar.
  ~Player();

  void run();

  // TODO: Make these private.
  // These two need to be public in order for the ugly hack in
  //  CanvasToolListen::playRange to work...
  volatile bool playing;
  volatile size_t pos;

public slots:
  /**
   * Set gain scalar.
   * This value is multiplied with each sample before it is played.
   */
  void setGainScalar(double gain_scalar);

  /**
   * Set gain is dB.
   * Convenience method for setGainScalar. It simple recalculates the dB value
   * to a scalar and sets the gain scalar accordingly.
   */
  void setGainDB(double gain_db);

signals:
  void peakUpdate(double peak);
  void positionUpdate(size_t position);

private slots:
  void reportTimeout();

private:
  ao_device *dev;  
  Canvas *canvas;

  QTimer report_timer;

  double gain_scalar;
  double peak;
};

#endif/*__DRUMGIZMO_PLAYER_H__*/

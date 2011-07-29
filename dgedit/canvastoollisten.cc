/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            canvastoollisten.cc
 *
 *  Fri Jul 29 16:57:48 CEST 2011
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
#include "canvastoollisten.h"

#define BUFSZ 1024

Player::Player(Canvas *c)
{
  canvas = c;
  playing = false;
  pos = 0;

  ao_initialize();

  ao_sample_format sf;
  sf.bits = 16;
  sf.rate = 44100;
  sf.channels = 1;
  sf.byte_format = AO_FMT_NATIVE;

  dev = ao_open_live(ao_default_driver_id(), &sf, 0);
}

Player::~Player()
{
  ao_close(dev);
  ao_shutdown();
}

void Player::run()
{
  while(true) {
    if(playing) {
      short s[BUFSZ];
      for(size_t i = 0; i < BUFSZ; i++) {
        if(i + pos < canvas->size) s[i] = canvas->data[pos + i] * ((2<<16) - 1);
        else {
          s[i] = 0;
          playing = false;
        }
      }

      ao_play(dev, (char*)s, BUFSZ * sizeof(short));
      canvas->update();

      pos += BUFSZ;

    } else {
      msleep(100);
    }
  }
}

CanvasToolListen::CanvasToolListen(Canvas *c)
  : player(c)
{
  canvas = c ;
  player.start();
}

bool CanvasToolListen::mousePressEvent(QMouseEvent *event)
{
  player.pos = canvas->unmapX(event->x());
  player.playing = true;
  return true;
}

bool CanvasToolListen::mouseReleaseEvent(QMouseEvent *event)
{
  player.playing = false;
  return true;
}

void CanvasToolListen::paintEvent(QPaintEvent *event, QPainter &painter)
{
  if(player.playing) {
    painter.setPen(QColor(0, 127, 127));
    painter.drawLine(canvas->mapX(player.pos),
                     event->rect().y(),
                     canvas->mapX(player.pos),
                     event->rect().y() + event->rect().height());
   }
}

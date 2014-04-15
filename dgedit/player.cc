/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            player.cc
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
#include "player.h"

#include <math.h>

#define BUFSZ 1024 * 2

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

  gain_scalar = 1000;

  peak = 0;

  connect(&report_timer, SIGNAL(timeout()), this, SLOT(reportTimeout()));
  report_timer.start(50); // Update 25 times per second
}

Player::~Player()
{
  ao_close(dev);
  ao_shutdown();
}

void Player::run()
{
  short s[BUFSZ];
  while(true) {
    if(playing) {
      for(size_t i = 0; i < BUFSZ; i++) {
        double sample = 0.0;
        if(i + pos < canvas->size) {
          sample = canvas->data[pos + i] * gain_scalar;
        } else {
          playing = false;
        }
        if(abs(sample) > peak) peak = abs(sample);
        s[i] = sample * SHRT_MAX;
      }

      ao_play(dev, (char*)s, BUFSZ * sizeof(short));

      pos += BUFSZ;

    } else {
      msleep(22);
    }
  }
}

void Player::setGainScalar(double g)
{
  gain_scalar = g / 10000.0;
}

void Player::setGainDB(double gain_db)
{
  setGainScalar(20*log10(gain_db));
}

void Player::reportTimeout()
{
  emit peakUpdate(peak);
  peak = 0.0;

  emit positionUpdate(pos);
  peak = 0.0;
}

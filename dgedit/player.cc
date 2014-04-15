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

Player::Player()
{
  playing = false;

  peak = 0;
  pos = 0;
  gain_scalar = 1.0;
  pcm_data = NULL;
  pcm_size = 0;

  connect(&report_timer, SIGNAL(timeout()), this, SLOT(reportTimeout()));
  report_timer.start(50); // Update 25 times per second
}

Player::~Player()
{
  running = false; // Signal player thread to stop
  wait(); // Wait for player thread to stop.
}

void Player::run()
{
  ao_initialize();

  ao_sample_format sf;
  sf.bits = 16;
  sf.rate = 44100;
  sf.channels = 1;
  sf.byte_format = AO_FMT_NATIVE;

  dev = ao_open_live(ao_default_driver_id(), &sf, 0);

  running = true;

  short s[BUFSZ];
  while(running) {
    if(playing) {
      for(size_t i = 0; i < BUFSZ; i++) {
        double sample = 0.0;
        if(i + pos < pcm_size) {
          sample = pcm_data[pos + i] * gain_scalar;
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

  ao_close(dev);
  ao_shutdown();
}

void Player::setGainScalar(double g)
{
  gain_scalar = g;
}

void Player::setGainDB(double gain_db)
{
  setGainScalar(pow(10, gain_db/ 20.0));
}

void Player::reportTimeout()
{
  emit peakUpdate(peak);
  emit peakUpdateDB(20 * log10(peak));
  peak = 0.0;

  emit positionUpdate(pos);
  peak = 0.0;
}

void Player::setPcmData(float *data, size_t size)
{
  pcm_data = data;
  pcm_size = size;
}

 void Player::setPosition(size_t position)
 {
   pos = position;
 }

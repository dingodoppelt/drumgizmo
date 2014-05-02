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

#define BUFSZ 512

Player::Player()
{
  peak = 0;
  pos = 0;
  gain_scalar = 1.0;
  pcm_data = NULL;
  pcm_size = 0;
  end = 0;
  new_selection = false;

  connect(&report_timer, SIGNAL(timeout()), this, SLOT(reportTimeout()));
  report_timer.start(50); // Update 25 times per second

  start();
}

Player::~Player()
{
  running = false; // Signal player thread to stop
  wait(); // Wait for player thread to stop.
}

#define _MIN(a, b) (a<b?a:b)
void Player::run()
{
  Selection sel;

  ao_initialize();

  ao_sample_format sf;
  memset(&sf, 0, sizeof(sf));
  sf.bits = 16;
  sf.rate = 44100;
  sf.channels = 1;
  sf.byte_format = AO_FMT_NATIVE;

  dev = ao_open_live(ao_default_driver_id(), &sf, 0);

  running = true;

  short s[BUFSZ];
  while(running) {

    { // Check for new Selection.
      QMutexLocker lock(&mutex);
      if(new_selection) {
        sel = selection;
        pos = sel.from;
        end = sel_end;
        new_selection = false;
      }
    }

    for(size_t i = 0; i < BUFSZ; i++) {
      double sample = 0.0;
      size_t p = i + pos;
      if(p < sel.to && p < end && p < pcm_size) {
        double fade = 1;
        if(p < (sel.from + sel.fadein)) {
          // Apply linear fade-in
          double fp = (double)((int)p - (int)sel.from);
          fade = fp / (double)sel.fadeout;
        }

        if(p > (sel.to - sel.fadeout)) {
          // Apply linear fade-out
          double fp = (double)(((int)sel.to - (int)sel.fadeout) - (int)p);
          fade = 1 + (fp / (double)sel.fadeout);
        }

        sample = pcm_data[p] * fade * gain_scalar;
      }
      if(fabs(sample) > peak) { 
        peak = fabs(sample);
      }
      s[i] = _MIN(sample * SHRT_MAX, SHRT_MAX);
    }
    
    ao_play(dev, (char*)s, BUFSZ * sizeof(short));

    pos += BUFSZ;
  }

  ao_close(dev);
  ao_shutdown();
}

bool Player::playSelectionDone()
{
  return pos >= sel_end || pos >= selection.to;
}

void Player::playSelection(Selection s, int len)
{
  { // Enqueue new Selection for player consumation
    QMutexLocker lock(&mutex);

    selection = s;

    if(len > 0) sel_end = len;
    else sel_end = selection.to - selection.from;
    
    sel_end += selection.from;

    new_selection = true;
  }

  // Wait until player actually consumed the new Selection.
  while(new_selection) {
    msleep(1);
  }
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
  Selection s;
  s.from = position;
  s.to = pcm_size;
  s.fadein = 0;
  s.fadeout = 0;
  playSelection(s);
}

void Player::stop()
{
  Selection s;
  s.from = 0;
  s.to = 0;
  s.fadein = 0;
  s.fadeout = 0;
  playSelection(s, pos);
}

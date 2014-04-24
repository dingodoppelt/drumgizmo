/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            powerlist.cc
 *
 *  Sun Jul 28 19:45:48 CEST 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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
#include "powerlist.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <hugin.hpp>

// Enable to calculate power on old samples without power attribute
//#define AUTO_CALCULATE_POWER

#define SIZE 500

// Box–Muller transform.
// See: http://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
static float box_muller_transform(float mean, float stddev)
{
  float U1 = (float)rand() / (float)RAND_MAX;
  float U2 = (float)rand() / (float)RAND_MAX;

  float x = sqrt(-2.0 * log(U1)) * cos(2.0 * M_PI * U2);

  return mean + stddev * x;
}

PowerList::PowerList()
{
  power_max = 0;
}

#define THRES 1.0

void PowerList::add(Sample *sample)
{
  PowerListItem item;
  item.power = -1;
  item.sample = sample;

  samples.push_back(item);
}

Channel *PowerList::getMasterChannel()
{
  std::map<Channel *, int> count;

  std::vector<PowerListItem>::iterator si = samples.begin();
  while(si != samples.end()) {
    PowerListItem &item = *si;
    Sample *sample = item.sample;

    Channel *max_channel = NULL;
    sample_t max_val = 0;

    // DEBUG(rand, "Sample: %s\n", sample->name.c_str());

    size_t ci = 0;
    AudioFiles::iterator ai = sample->audiofiles.begin();
    while(ai != sample->audiofiles.end()) {
      Channel *c = ai->first;
      AudioFile *af = ai->second;

      af->load(SIZE);

      float silence = 0;
      size_t silence_length = 4;
      for(size_t s = af->size; s > 0 && s > af->size - silence_length; s--) {
        silence += af->data[s];
      }
      silence /= silence_length;

      size_t s = 0;
      for(; s < af->size; s++) {
        float val = af->data[s] * af->data[s] * (1.0 / (float)(s+1));
        if(val > max_val) {
          max_val = val;
          max_channel = c;
          break;
        }
      }
      
      af->unload();

      ai++;
      ci++;
    }

    if(max_channel) {
      if(count.find(max_channel) == count.end()) count[max_channel] = 0;
      count[max_channel]++;
    }

    si++;
  }

  Channel *master = NULL;
  int max_count = -1;

  std::map<Channel *, int>::iterator ci = count.begin();
  while(ci != count.end()) {
    if(ci->second > max_count &&
       strstr(ci->first->name.c_str(), "Alesis") == 0) {
      master = ci->first;
      max_count = ci->second;
    }
    ci++;
  }

  return master;
}

void PowerList::finalise()
{
#ifdef AUTO_CALCULATE_POWER
  Channel *master_channel = getMasterChannel();

  if(master_channel == NULL) {
    ERR(rand, "No master channel found!\n");
    return; // This should not happen...
  }

  DEBUG(rand, "Master channel: %s\n", master_channel->name.c_str());
#endif/*AUTO_CALCULATE_POWER*/

  std::vector<PowerListItem>::iterator si = samples.begin();
  while(si != samples.end()) {
    PowerListItem &item = *si;
    Sample *sample = item.sample;

 #ifdef AUTO_CALCULATE_POWER
   DEBUG(rand, "Sample: %s\n", sample->name.c_str());

    AudioFile *master = NULL;

    AudioFiles::iterator afi = sample->audiofiles.begin();
    while(afi != sample->audiofiles.end()) {
      if(afi->first->name == master_channel->name) {
        master = afi->second;
        break;
      }
      afi++;
    }

    if(master == NULL) {
      si++;
      continue;
    }

    master->load();
#endif/*AUTO_CALCULATE_POWER*/

#ifdef AUTO_CALCULATE_POWER
    if(sample->power == -1) { // Power not defined. Calculate it!
      DEBUG(powerlist, "Calculating power\n");

      float power = 0;
      size_t s = 0;
      for(; s < SIZE && s < master->size; s++) {
        power += master->data[s] * master->data[s];
      }
    
      power = sqrt(power);

      if(power > power_max) power_max = power;

      sample->power = power;
    }
#endif/*AUTO_CALCULATE_POWER*/

    item.power = sample->power;

    DEBUG(rand, " - power: %f\n", item.power);
    
    si++;
  }
}

Sample *PowerList::get(level_t level)
{
  Sample *sample = NULL;
  float power = 0;

  float mean = level * power_max;
  float stddev = power_max / samples.size() * 1.5;

  float lvl = box_muller_transform(mean, stddev);

  DEBUG(rand, "lvl: %f (mean: %.2f, stddev: %.2f)\n",
        lvl, mean, stddev);

  std::vector<PowerListItem>::iterator i = samples.begin();
  while(i != samples.end()) {
    if(sample == NULL) {
      sample = i->sample;
      power = i->power;
    }

    if(fabs(i->power - lvl) < fabs(power - lvl)) {
      sample = i->sample;
      power = i->power;
    }

    i++;
  }

  DEBUG(rand, "Found power %f\n", power);

  return sample;
}

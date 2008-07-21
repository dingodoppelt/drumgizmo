/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            midimapper.cc
 *
 *  Mon Jul 21 15:24:08 CEST 2008
 *  Copyright 2008 Bent Bisballe Nyeng
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
#include "midimapper.h"

#define NOTE_ON 0x90

MidiMapper::MidiMapper()
{
  for(int i = 0; i < 16; i++) _map[i] = i % 2;
}

//http://ccrma-www.stanford.edu/~craig/articles/linuxmidi/misc/essenmidi.html
int MidiMapper::map(jack_midi_event_t event)
{
  // Parse midi event
  printf("[ Time: %d  Size: %d  ", event.time, event.size);
  for(size_t j = 0; j < event.size; j++) {
    jack_midi_data_t m = event.buffer[j];
    printf("  Data: %d ", m);
  }
  printf("]\n");

  if(event.size == 3) return -1;
  if(event.buffer[0] != NOTE_ON) return -1;

  int key = event.buffer[1];
  //  int velocity = event.buffer[2];

  if(_map.find(key) == _map.end()) return -1; // key is not in map.

  return _map[key];
}

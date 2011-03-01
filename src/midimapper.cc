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

#include <stdio.h>

#define NOTE_ON 0x9

MidiMapper::MidiMapper(DrumKit *drumkit)
{
  this->drumkit = drumkit;
  /*
35  Acoustic Bass Drum  59  Ride Cymbal 2
36  Bass Drum 1         60  Hi Bongo
37  Side Stick          61  Low Bongo
38  Acoustic Snare      62  Mute Hi Conga
39  Hand Clap           63  Open Hi Conga
40  Electric Snare      64  Low Conga
41  Low Floor Tom       65  High Timbale
42  Closed Hi-Hat       66  Low Timbale
43  High Floor Tom      67  High Agogo
44  Pedal Hi-Hat        68  Low Agogo
45  Low Tom             69  Cabasa
46  Open Hi-Hat         70  Maracas
47  Low-Mid Tom         71  Short Whistle
48  Hi-Mid Tom          72  Long Whistle
49  Crash Cymbal 1      73  Short Guiro
50  High Tom            74  Long Guiro
51  Ride Cymbal 1       75  Claves
52  Chinese Cymbal      76  Hi Wood Block
53  Ride Bell           77  Low Wood Block
54  Tambourine          78  Mute Cuica
55  Splash Cymbal       79  Open Cuica
56  Cowbell             80  Mute Triangle
57  Crash Cymbal 2      81  Open Triangle
58  Vibraslap
  */
}

//http://ccrma-www.stanford.edu/~craig/articles/linuxmidi/misc/essenmidi.html
Sample *MidiMapper::map(jack_midi_event_t event)
{
  Sample *sample = NULL;
#if 0
  //  printf("m"); fflush(stdout);

  if(event.size != 3) return NULL;
  if((event.buffer[0] & NOTE_ON) != NOTE_ON) return NULL;

  int key = event.buffer[1];
  int velocity = event.buffer[2];

  if(velocity == 0) return NULL;
  /*
  // Parse midi event
  printf("[ Time: %d  Size: %d  ", event.time, event.size);
  for(size_t j = 0; j < event.size; j++) {
    jack_midi_data_t m = event.buffer[j];
    printf("  Data: %d ", m);
  }
  printf("]\n");
  */
  
  if(drumkit->instruments.find(key) == drumkit->instruments.end()) {
    printf("Unknown instrument %d\n", key);
    return NULL;
  }
  
  Velocity *v = drumkit->instruments[key]->getVelocity(velocity);
  
  if(!v) return NULL;
  sample = v->getSample();
#endif
  return sample;
}

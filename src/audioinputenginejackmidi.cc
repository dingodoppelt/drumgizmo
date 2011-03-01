/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audioinputenginejackmidi.cc
 *
 *  Sun Feb 27 11:33:31 CET 2011
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
#include "audioinputenginejackmidi.h"

#define NOTE_ON 0x90

extern "C" {
  static int _wrap_jack_process(jack_nframes_t nframes, void *arg){
    return ((AudioInputEngineJackMidi*)arg)->process(nframes);}
}

AudioInputEngineJackMidi::AudioInputEngineJackMidi()
{
  pos = 0;
}

AudioInputEngineJackMidi::~AudioInputEngineJackMidi()
{
  //  wait_stop();
  jack_client_close(jack_client);
}

int AudioInputEngineJackMidi::process(jack_nframes_t nframes)
{
  //  printf("               jk: %d\n", pos);

  void *midibuffer = jack_port_get_buffer(midi_port, nframes);

  jack_nframes_t midievents = jack_midi_get_event_count(midibuffer);
  //  if(midievents) printf("#%d\n", midievents);
  for(jack_nframes_t i = 0; i < midievents; i++) {
    jack_midi_event_t event;
    jack_midi_event_get(&event, midibuffer, i);
    
    
    if(event.size != 3) continue;
    if((event.buffer[0] & NOTE_ON) != NOTE_ON) continue;
    
    int key = event.buffer[1];
    int velocity = event.buffer[2];
    
    //if(velocity == 0) continue;
    
    printf("Event key:%d vel:%d\n", key, velocity);
    Event *evt = new EventSine(0, key * 10, (float)velocity / 127.0, 1000);
    eventqueue->post(evt, pos + event.time);
  }
  
  jack_midi_clear_buffer(midibuffer);
    
  pos += nframes;

  return 0;
}

bool AudioInputEngineJackMidi::init(EventQueue *e)
{
  eventqueue = e;

	jack_status_t status;

	jack_client = jack_client_open("DrumGizmo", JackNullOption, &status);

	midi_port = jack_port_register(jack_client,
                                 "drumgizmo_midiin",
                                 JACK_DEFAULT_MIDI_TYPE,
                                 JackPortIsInput,// | JackPortIsTerminal,
                                 0);

  jack_set_process_callback(jack_client, _wrap_jack_process, this);

  jack_activate(jack_client);

  return true;
}

#ifdef TEST_AUDIOINPUTENGINEJACKMIDI
//Additional dependency files
//deps:
//Required cflags (autoconf vars may be used)
//cflags:
//Required link options (autoconf vars may be used)
//libs:
#include "test.h"

TEST_BEGIN;

// TODO: Put some testcode here (see test.h for usable macros).

TEST_END;

#endif/*TEST_AUDIOINPUTENGINEJACKMIDI*/

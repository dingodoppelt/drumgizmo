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

AudioInputEngineJackMidi::AudioInputEngineJackMidi()
{
  jackclient = init_jack_client();

  jackclient->addJackProcess(this);

  pos = 0;
}

AudioInputEngineJackMidi::~AudioInputEngineJackMidi()
{
  jackclient->removeJackProcess(this);
  close_jack_client();
}

void AudioInputEngineJackMidi::jack_process(jack_nframes_t nframes)
{
  void *midibuffer = jack_port_get_buffer(midi_port, nframes);

  jack_nframes_t midievents = jack_midi_get_event_count(midibuffer);

  for(jack_nframes_t i = 0; i < midievents; i++) {
    jack_midi_event_t event;
    jack_midi_event_get(&event, midibuffer, i);
    
    if(event.size != 3) continue;
    if((event.buffer[0] & NOTE_ON) != NOTE_ON) continue;
    
    int key = event.buffer[1];
    int velocity = event.buffer[2];
    
    printf("Event key:%d vel:%d\n", key, velocity);
    /*
    if(kit->midimap.find(key) == kit->midimap.end()) {
      printf("Missing note %d in midimap.\n", key);
      continue;
    }

    std::string instr = kit->midimap[key];

    if(kit->instruments.find(instr) == kit->instruments.end()) {
      printf("Missing instrument %s.\n", instr.c_str());
      continue;
    }
    */

    Instrument *i = NULL;
    int d = key % kit->instruments.size();
    Instruments::iterator it = kit->instruments.begin();
    while(d--) {
      i = &(it->second);
      it++;
    }

    if(i == NULL) {
      continue;
    }

    //    Sample *s = i.sample((float)velocity/127.0);
    Sample *s = i->sample(0.5);
    
    if(s == NULL) {
      printf("Missing Sample.\n");
      continue;
    }

    Channels::iterator j = kit->channels.begin();
    while(j != kit->channels.end()) {
      Channel &ch = *j;
      AudioFile *af = s->getAudioFile(&ch);
      if(af == NULL) {
        printf("Missing AudioFile.\n");
      } else {
        printf("Adding event.\n");
        Event *evt = new EventSample(ch.num, 1.0, af);
        eventqueue->post(evt, pos + event.time + nframes);
      }
      j++;
    }
  }
  
  jack_midi_clear_buffer(midibuffer);
    
  pos += nframes;
}

bool AudioInputEngineJackMidi::init(EventQueue *e, DrumKit *dk)
{
  eventqueue = e;
  kit = dk;

	midi_port = jack_port_register(jackclient->jack_client,
                                 "drumgizmo_midiin",
                                 JACK_DEFAULT_MIDI_TYPE,
                                 JackPortIsInput,// | JackPortIsTerminal,
                                 0);

  return true;
}


bool AudioInputEngineJackMidi::activate()
{
  jackclient->activate();
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

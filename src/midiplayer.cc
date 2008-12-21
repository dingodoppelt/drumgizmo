/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            midiplayer.cc
 *
 *  Sat Jul 26 15:23:19 CEST 2008
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
#include "midiplayer.h"

#define NOTE_ON 0x90

int _process(jack_nframes_t nframes, void *arg)
{
  return ((MidiPlayer*)arg)->process(nframes);
}

MidiPlayer::MidiPlayer(std::string filename)
{
	jack_status_t status;

  jack_client_t *jack_client = jack_client_open("MidiTest", JackNullOption, &status);

	port = jack_port_register(jack_client,
                            "midi_out",
                            JACK_DEFAULT_MIDI_TYPE,
                            JackPortIsOutput,// | JackPortIsTerminal,
                            0);

  jack_set_process_callback(jack_client, _process, this);

	jack_activate(jack_client);

  jack_connect(jack_client, "MidiTest:midi_out", "DrumGizmo:midi_in");

  jack_connect(jack_client, "DrumGizmo:Kick-R", "system:playback_1");
  jack_connect(jack_client, "DrumGizmo:Kick-L", "system:playback_1");
  jack_connect(jack_client, "DrumGizmo:SnareTop", "system:playback_1");
  jack_connect(jack_client, "DrumGizmo:SnareBottom", "system:playback_1");
  jack_connect(jack_client, "DrumGizmo:SnareTrigger", "system:playback_1");
  jack_connect(jack_client, "DrumGizmo:Tom1", "system:playback_1");
  jack_connect(jack_client, "DrumGizmo:Tom2", "system:playback_1");
  jack_connect(jack_client, "DrumGizmo:Tom3", "system:playback_1");
  jack_connect(jack_client, "DrumGizmo:Tom4", "system:playback_1");
  jack_connect(jack_client, "DrumGizmo:Ride", "system:playback_1");
  //  jack_connect(jack_client, "DrumGizmo:Hihat", "system:playback_1");

  jack_connect(jack_client, "DrumGizmo:Kick-R", "system:playback_2");
  jack_connect(jack_client, "DrumGizmo:Kick-L", "system:playback_2");
  jack_connect(jack_client, "DrumGizmo:SnareTop", "system:playback_2");
  jack_connect(jack_client, "DrumGizmo:SnareBottom", "system:playback_2");
  jack_connect(jack_client, "DrumGizmo:SnareTrigger", "system:playback_2");
  jack_connect(jack_client, "DrumGizmo:Tom1", "system:playback_2");
  jack_connect(jack_client, "DrumGizmo:Tom2", "system:playback_2");
  jack_connect(jack_client, "DrumGizmo:Tom3", "system:playback_2");
  jack_connect(jack_client, "DrumGizmo:Tom4", "system:playback_2");
  jack_connect(jack_client, "DrumGizmo:Ride", "system:playback_2");
  //  jack_connect(jack_client, "DrumGizmo:Hihat", "system:playback_2");

  jack_connect(jack_client, "DrumGizmo:OH-R", "system:playback_1");
  jack_connect(jack_client, "DrumGizmo:OH-L", "system:playback_2");
  jack_connect(jack_client, "DrumGizmo:Amb-R", "system:playback_1");
  jack_connect(jack_client, "DrumGizmo:Amb-L", "system:playback_2");

  timer = 0;
  next = 44100;
}

MidiPlayer::~MidiPlayer()
{
}

#if 0 // All
#define NUM_INST 11
static int inst[] = { 35, 36, 38, 46, 41, 43, 45, 47, 49, 57, 51 };
#endif

#if 1 // Cymbals
#define NUM_INST 3
static int inst[] = { 51, 49, 57 };
#endif

#if 0 // Toms
#define NUM_INST 4
static int inst[] = { 41, 43, 45, 47 };
#endif

#if 0 // Kicks
#define NUM_INST 2
static int inst[] = { 35, 36 };
#endif
int MidiPlayer::process(jack_nframes_t nframes)
{
  return 0;

  //  if(jack_port_connected_to(test_midi_port, "DrumGizmo:midi_in")) {
  void* port_buf = jack_port_get_buffer(port, nframes);

  if(timer > next) { // activate every second (44100 samples)
    //    printf("ding\n");

    jack_nframes_t time = (jack_nframes_t)(((float)rand() / (float)RAND_MAX) * nframes);
    size_t size = 3;
    jack_midi_data_t note[] = { NOTE_ON, inst[rand() % NUM_INST], rand() % 127};
    jack_midi_event_write(port_buf, time, note, size);

    timer = 0;
    next = (size_t)(((float)rand() / (float)RAND_MAX) * 0.2 * 44100);
  }

  timer += nframes;
  
  return 0;
}



/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumgizmo.cc
 *
 *  Sun Jul 20 19:25:01 CEST 2008
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
#include "jackclient.h"

#include <jack/jack.h>
#include <jack/midiport.h>

static jack_port_t *test_midi_port = NULL;
static size_t timer = 0;
static size_t next = 44100;

int process(jack_nframes_t nframes, void *arg)
{
  //  if(jack_port_connected_to(test_midi_port, "DrumGizmo:midi_in")) {
  void* port_buf = jack_port_get_buffer(test_midi_port, nframes);

  if(timer > next) { // activate every second (44100 samples)
    //    printf("ding\n");

    jack_nframes_t time = (jack_nframes_t)(((float)rand() / (float)RAND_MAX) * nframes);
    size_t size = 1;
    jack_midi_data_t all_notes_off[] = { rand() % 2 };
    jack_midi_event_write(port_buf, time, all_notes_off, size);

    timer = 0;
    next = (size_t)(((float)rand() / (float)RAND_MAX) * 0.2 * 44100);
  }

  timer += nframes;
  
  return 0;
}

void sendMidi()
{
	jack_status_t status;

  jack_client_t *jack_client = jack_client_open("MidiTest", JackNullOption, &status);

	test_midi_port = jack_port_register(jack_client,
                                      "midi_out",
                                      JACK_DEFAULT_MIDI_TYPE,
                                      JackPortIsOutput,// | JackPortIsTerminal,
                                      0);

  jack_set_process_callback(jack_client, process, NULL);

	jack_activate(jack_client);

  jack_connect(jack_client, "MidiTest:midi_out", "DrumGizmo:midi_in");

  jack_connect(jack_client, "DrumGizmo:output_1", "system:playback_1");
  jack_connect(jack_client, "DrumGizmo:output_2", "system:playback_2");
}

int main(int argc, char *argv[])
{
  JackClient client;

  client.activate();

  //  sendMidi();

  while(1) sleep(1);

  return 0;
}


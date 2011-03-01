/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audiooutputenginejack.cc
 *
 *  Thu Sep 16 10:28:37 CEST 2010
 *  Copyright 2010 Bent Bisballe Nyeng
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
#include "audiooutputenginejack.h"

#include <string.h>
#include "drumgizmo.h"

extern "C" {
  static int _wrap_jack_process(jack_nframes_t nframes, void *arg){
    return ((AudioOutputEngineJack*)arg)->process(nframes);}
}

AudioOutputEngineJack::AudioOutputEngineJack()
{
	jack_status_t status;

	jack_client = jack_client_open("DrumGizmo", JackNullOption, &status);
  jack_set_process_callback(jack_client, _wrap_jack_process, this);
}

AudioOutputEngineJack::~AudioOutputEngineJack()
{
	jack_client_close(jack_client);
}

bool AudioOutputEngineJack::init(Channels *channels)
{
  this->channels = channels;

  Channels::iterator i = channels->begin();
  while(i != channels->end()) {

    jack_port_t *port =
      jack_port_register(jack_client,
                         i->name.c_str(),
                         JACK_DEFAULT_AUDIO_TYPE,
                         JackPortIsOutput | JackPortIsTerminal,
                         0);

    output_ports.push_back(port);
    i->num = output_ports.size() - 1;

    i++;
  }

  return true;
}

void AudioOutputEngineJack::run(DrumGizmo *drumgizmo)
{
  gizmo = drumgizmo;

  jack_activate(jack_client);

  jack_connect(jack_client, "DrumGizmo:port0", "system:playback_1");
  jack_connect(jack_client, "DrumGizmo:port1", "system:playback_2");

  while(drumgizmo->isRunning()) {
    sleep(1);
  }
}

int AudioOutputEngineJack::process(jack_nframes_t nframes)
{
  gizmo->pre(nframes);

  Channels::iterator i = channels->begin();
  while(i != channels->end()) {
    jack_port_t *port = output_ports[i->num];
    jack_default_audio_sample_t *buffer;
    buffer = (jack_default_audio_sample_t *)jack_port_get_buffer(port, nframes);
    
    memset(buffer, 0, nframes * sizeof(sample_t));
    gizmo->getSamples(&(*i), buffer, nframes);
    
    i++;
  }

  gizmo->post(nframes);

  return 0;
}

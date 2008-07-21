/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            jackclient.h
 *
 *  Sun Jul 20 21:48:44 CEST 2008
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
#ifndef __DRUMGIZMO_JACKCLIENT_H__
#define __DRUMGIZMO_JACKCLIENT_H__

#include <vector>

#include <jack/jack.h>
#include <jack/midiport.h>

#include "event.h"
#include "sample.h"
#include "midimapper.h"

typedef std::vector< jack_port_t *> Ports;

class JackClient {
public:
  JackClient(size_t num_inputs = 16, size_t num_outputs = 16);
  ~JackClient();

  void activate();

  // Callbacks
  void shutdown();
  int process(jack_nframes_t nframes);
  void thread_init();
  void freewheel_mode(int freewheel_mode);
  int buffer_size(jack_nframes_t nframes);
  int sample_rate(jack_nframes_t nframes);
  void port_registration(jack_port_id_t port, int i);
  int graph_order();
  int xrun();

private:
	jack_client_t *jack_client;
  Ports input_ports;
  Ports output_ports;
  jack_port_t *midi_port;

  Sample *sample[32];
  Events events;

  MidiMapper midimapper;
};

#endif/*__DRUMGIZMO_JACKCLIENT_H__*/

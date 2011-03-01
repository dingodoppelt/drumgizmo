/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audioinputenginejackmidi.h
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
#ifndef __DRUMGIZMO_AUDIOINPUTENGINEJACKMIDI_H__
#define __DRUMGIZMO_AUDIOINPUTENGINEJACKMIDI_H__

#include <jack/jack.h>
#include <jack/midiport.h>

#include "audioinputengine.h"
#include "event.h"

class AudioInputEngineJackMidi : public AudioInputEngine {
public:
  AudioInputEngineJackMidi();
  ~AudioInputEngineJackMidi();

  bool init(EventQueue *e);
  void run(size_t pos, size_t len) {}

  void thread_main();

  int process(jack_nframes_t nframes);

private:
  size_t pos;
  EventQueue *eventqueue;
  jack_client_t *jack_client;
  jack_port_t *midi_port;
};

#endif/*__DRUMGIZMO_AUDIOINPUTENGINEJACKMIDI_H__*/

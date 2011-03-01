/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audiooutputenginejack.h
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
#ifndef __DRUMGIZMO_AUDIOOUTPUTENGINEJACK_H__
#define __DRUMGIZMO_AUDIOOUTPUTENGINEJACK_H__

#include <vector>

#include <jack/jack.h>

#include "audiooutputengine.h"

class AudioOutputEngineJack : public AudioOutputEngine {
public:
  AudioOutputEngineJack();
  ~AudioOutputEngineJack();

  bool init(Channels *channels);

  void run(DrumGizmo *drumgizmo);

  // Internal callback method. *must* be public.
  int process(jack_nframes_t nframes);

private:
	jack_client_t *jack_client;
  std::vector< jack_port_t *> output_ports;

  jack_port_t *port0;
  jack_port_t *port1;

  DrumGizmo *gizmo;
};


#endif/*__DRUMGIZMO_AUDIOOUTPUTENGINEJACK_H__*/

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            instrument.h
 *
 *  Tue Jul 22 17:14:19 CEST 2008
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
#ifndef __DRUMGIZMO_INSTRUMENT_H__
#define __DRUMGIZMO_INSTRUMENT_H__

#include <string>
#include <vector>

#include <jack/jack.h>

#include "velocity.h"

class Instrument {
public:
  Instrument(std::string name, unsigned int midimap);

  void addVelocity(Velocity *velocity);
  Velocity *getVelocity(unsigned int velocity);

  std::string name;
  unsigned int midimap;

  jack_port_t *port;

private:
  typedef std::vector< Velocity * > Velocities;
  Velocities velocities;
};

#endif/*__DRUMGIZMO_INSTRUMENT_H__*/

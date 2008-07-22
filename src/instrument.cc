/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            instrument.cc
 *
 *  Tue Jul 22 17:14:20 CEST 2008
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
#include "instrument.h"

Instrument::Instrument(std::string name, unsigned int midimap)
{
  this->name = name;
  this->midimap = midimap;
}

void Instrument::addVelocity(Velocity *velocity)
{
  velocities.push_back(velocity);
}

Velocity *Instrument::getVelocity(unsigned int v)
{
  Velocities::iterator i = velocities.begin();
  while(i != velocities.end()) {
    if(v >= (*i)->lower && v <= (*i)->upper) return *i;
    i++;
  }
  return NULL;
}

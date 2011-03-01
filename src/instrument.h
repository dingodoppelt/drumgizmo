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

#include "rangemap.h"
#include "sample.h"

class Instrument {
public:
  Instrument(std::string name);

  Sample *sample(level_t level);

  void addSample(level_t a, level_t b, Sample *s);

private:
  std::string name;
  RangeMap<level_t, Sample*> samples;
};

typedef std::map< std::string, Instrument > Instruments;

/*
 * <?xml version='1.0' encoding='UTF-8'?>
 * <instrument name="kick-r">
 *  <samples>
 *   <sample name="kick-r-1">
 *    <audiofile channel="Alesis-3" file="samples/1-kick-r-Alesis-3.wav"/>
 *    <audiofile channel="Amb L-3" file="samples/1-kick-r-Amb L-3.wav"/>
 *    <audiofile channel="Amb R-3" file="samples/1-kick-r-Amb R-3.wav"/>
 *    <audiofile channel="Kick L-3" file="samples/1-kick-r-Kick L-3.wav"/>
 *    <audiofile channel="Kick R-3" file="samples/1-kick-r-Kick R-3.wav"/>
 *   </sample>
 *   <sample name="kick-r-2">
 *    <audiofile channel="Alesis-3" file="samples/2-kick-r-Alesis-3.wav"/>
 *    <audiofile channel="Amb L-3" file="samples/2-kick-r-Amb L-3.wav"/>
 *    <audiofile channel="Amb R-3" file="samples/2-kick-r-Amb R-3.wav"/>
 *    <audiofile channel="Kick L-3" file="samples/2-kick-r-Kick L-3.wav"/>
 *    <audiofile channel="Kick R-3" file="samples/2-kick-r-Kick R-3.wav"/>
 *   </sample>
 *   <sample name="kick-r-3">
 *    <audiofile channel="Alesis-3" file="samples/3-kick-r-Alesis-3.wav"/>
 *    <audiofile channel="Amb L-3" file="samples/3-kick-r-Amb L-3.wav"/>
 *    <audiofile channel="Amb R-3" file="samples/3-kick-r-Amb R-3.wav"/>
 *    <audiofile channel="Kick L-3" file="samples/3-kick-r-Kick L-3.wav"/>
 *    <audiofile channel="Kick R-3" file="samples/3-kick-r-Kick R-3.wav"/>
 *   </sample>
 *  </samples>
 *  <velocities>
 *   <velocity lower="0" upper="99">
 *    <sampleref name="kick-r-1"/>
 *    <sampleref name="kick-r-2"/>
 *   </velocity>
 *   <velocity lower="100" upper="127">
 *    <sampleref name="kick-r-3"/>
 *   </velocity>
 *  </velocities>
 * </instrument>
 */

#endif/*__DRUMGIZMO_INSTRUMENT_H__*/

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audioinputenginemidi.cc
 *
 *  Mon Apr  1 20:13:25 CEST 2013
 *  Copyright 2013 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "audioinputenginemidi.h"

#include "midimapparser.h"

#include "drumgizmo.h"

#include <hugin.hpp>

AudioInputEngineMidi::AudioInputEngineMidi()
  : refs(REFSFILE)
{
  is_valid = false;
}

bool AudioInputEngineMidi::loadMidiMap(std::string file, Instruments &instruments)
{
  std::string f = file;

  if(refs.load()) {
    if(file.size() > 1 && file[0] == '@') {
      f = refs.getValue(file.substr(1));
    }
  } else {
    ERR(drumkitparser, "Error reading refs.conf");
  }

  midimap = "";
  is_valid = false;

  DEBUG(mmap, "loadMidiMap(%s, i.size() == %d)\n", f.c_str(),
        (int)instruments.size());

  if(f == "") return false;

  MidiMapParser p;
  if(p.parseFile(f)) {
    return false;
  }

  mmap.clear();
  mmap.midimap = p.midimap;

  for(size_t i = 0; i < instruments.size(); i++) {
    mmap.instrmap[instruments[i]->name()] = i;
  }

  midimap = file;
  is_valid = true;

  return true;
}

std::string AudioInputEngineMidi::midimapFile()
{
  return midimap;
}

bool AudioInputEngineMidi::isValid()
{
  return is_valid;
}

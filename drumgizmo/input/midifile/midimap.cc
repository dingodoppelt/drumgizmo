/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            midimap.cc
 *
 *  Mon Jun 13 21:36:30 CEST 2011
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
#include "midimap.h"

/*

  35 = 1
  36-51 = 2            - TODO!
  51, 53 = 3           - TODO!
  54, 56-59, 62 = 4    - TODO!

 */

#include <stdio.h>
#include <stdlib.h>

MidiMap::MidiMap()
{
}

bool MidiMap::load(std::string file)
{
  FILE *fp = fopen(file.c_str(), "r");
  if(!fp) return false;

  std::string line;
  while(!feof(fp)) {
    int c = fgetc(fp);
    //    printf("[%c]\n", c);
    if(c == '\n') {
      if(line != "") {
        int from = atoi(line.substr(0, line.find('=')).c_str());
        int to = atoi(line.substr(line.find('=')+1,
                                  line.length()-line.find('=')+1).c_str());
        map[from] = to;
        // printf("Line: '%s', from: %d to: %d\n", line.c_str(), from, to);
        
      }
      line = "";
    } else {
      if((c >= '0' && c <= '9') || c == ',' || c == '-' || c == '=') {
        line += (char)c;
      } else {
        if(c != '\t' && c != ' ') return false; // Parse error.
      }
    }
  }

  return true;
}

int MidiMap::lookup(int note)
{
  return map[note];
}

#ifdef TEST_MIDIMAP
//Additional dependency files
//deps:
//Required cflags (autoconf vars may be used)
//cflags:
//Required link options (autoconf vars may be used)
//libs:
#include "test.h"

TEST_BEGIN;

// TODO: Put some testcode here (see test.h for usable macros).

TEST_END;

#endif/*TEST_MIDIMAP*/

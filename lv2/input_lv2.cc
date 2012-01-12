/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            input_lv2.cc
 *
 *  Wed Jul 13 14:27:02 CEST 2011
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
#include "input_lv2.h"

#include <midimapparser.h>

InputLV2::InputLV2()
{
  eventPort = NULL;
}

InputLV2::~InputLV2()
{
}

bool InputLV2::init(Instruments &i)
{
  instruments = &i;
  return true;
}

void InputLV2::setParm(std::string parm, std::string value)
{
}

bool InputLV2::start()
{
  return true;
}

void InputLV2::stop()
{
}

void InputLV2::pre()
{
}
event_t *InputLV2::run(size_t pos, size_t len, size_t *nevents)
{
  event_t *list;
  size_t listsize;

  list = (event_t *)malloc(sizeof(event_t) * 1000);
  listsize = 0;

  LV2_Event_Iterator iterator;
	for(lv2_event_begin(&iterator, eventPort);
      lv2_event_is_valid(&iterator);
      lv2_event_increment(&iterator)) {

    LV2_Event* ev = lv2_event_get(&iterator, NULL);

    uint8_t* const data = (uint8_t* const)(ev + 1);

    if ((data[0] & 0xF0) == 0x90) {

    int key = data[1];
    int velocity = data[2];
    
    printf("Event key:%d vel:%d\n", key, velocity);
    
    int i = mmap.lookup(key);
    if(velocity && i != -1) {
      list[listsize].type = TYPE_ONSET;
      list[listsize].instrument = i;
      list[listsize].velocity = velocity / 127.0;
      list[listsize].offset = ev->frames;
      listsize++;
    }
      /*
				start_frame = ev->frames;
				plugin->frame = 0;
				plugin->play  = true;
      */
    }
  }

  *nevents = listsize;
  return list;
}

void InputLV2::post()
{
}

void InputLV2::loadMidiMap(std::string f)
{
  MidiMapParser p(f);
  if(p.parse()) {/*return false;*/}
  mmap.midimap = p.midimap;

  for(size_t i = 0; i < instruments->size(); i++) {
    mmap.instrmap[(*instruments)[i]->name()] = i;
  }
}

#ifdef TEST_INPUT_LV2
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

#endif/*TEST_INPUT_LV2*/

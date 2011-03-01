/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            event.h
 *
 *  Sat Sep 18 22:02:16 CEST 2010
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
#ifndef __DRUMGIZMO_EVENT_H__
#define __DRUMGIZMO_EVENT_H__

#include <map>
#include <stdio.h>
#include <string>
#include <sndfile.h>

#include "audio.h"
#include "mutex.h"

typedef unsigned int timepos_t;

class Event {
public:
  typedef enum {
    sine,
    noise,
    sample
  } type_t;
  
  virtual type_t type() = 0;

  channel_t channel;
  timepos_t offset;
};

class EventSine : public Event {
public:
  EventSine(channel_t c, float f, float g, timepos_t l)
  {
    channel = c;
    freq = f;
    gain = g;
    len = l;
    t = 0;
  }
  Event::type_t type() { return Event::sine; }

  float freq;
  float gain;
  timepos_t len;

  unsigned int t;
};

class EventNoise : public Event {
public:
  EventNoise(channel_t c, float g, timepos_t l)
  {
    channel = c;
    gain = g;
    len = l;
    t = 0;
  }
  Event::type_t type() { return Event::noise; }

  float gain;
  timepos_t len;

  unsigned int t;
};

class EventSample : public Event {
public:
  EventSample(channel_t c, float g, std::string f)
  {
    channel = c;
    gain = g;
    t = 0;
    file = f;
  }

  Event::type_t type() { return Event::sample; }

  float gain;

  unsigned int t;

  std::string file;
};



class EventQueue {
public:
  void post(Event *event, timepos_t time);
  Event *take(timepos_t time);
  bool hasEvent(timepos_t time);
  size_t size() { return queue.size(); }

private:
  std::multimap< timepos_t, Event* > queue;
  Mutex mutex;
};

#endif/*__DRUMGIZMO_EVENT_H__*/

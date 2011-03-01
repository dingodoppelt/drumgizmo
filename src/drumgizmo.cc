/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumgizmo.cc
 *
 *  Thu Sep 16 10:24:40 CEST 2010
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
#include "drumgizmo.h"

#include <math.h>
#include <stdio.h>

#include "drumkitparser.h"
#include "audiooutputengine.h"
#include "audioinputengine.h"
#include "event.h"

DrumGizmo::DrumGizmo(AudioOutputEngine &o,
                     AudioInputEngine &i,
                     ChannelMixer &m)
  : mixer(m), oe(o), ie(i)
{
  time = 0;
  Channel c1; c1.num = 0; c1.name="left";
  Channel c2; c2.num = 1; c2.name="right";
  channels.push_back(c1);
  channels.push_back(c2);
}

DrumGizmo::~DrumGizmo()
{
  AudioFiles::iterator i = audiofiles.begin();
  while(i != audiofiles.end()) {
    AudioFile *audiofile = i->second;
    delete audiofile;
    i++;
  }
}

bool DrumGizmo::loadkit(const std::string &kitfile)
{
  /*
  DrumKitParser parser(kitfile, kit);
  if(parser.parse()) return false;
  */

  return true;
}

bool DrumGizmo::init(bool preload)
{
  if(preload) {
    AudioFiles::iterator i = audiofiles.begin();
    while(i != audiofiles.end()) {
      AudioFile *audiofile = i->second;
      audiofile->load();
      i++;
    }
  }

  if(!oe.init(&channels)) return false;

  if(!ie.init(&eventqueue)) return false;

  return true;
}

void DrumGizmo::run()
{
  is_running = true;
  oe.run(this);
}

void DrumGizmo::pre(size_t sz)
{
  ie.run(time, sz);

  for(size_t n = 0; n < sz; n++) {
    while(eventqueue.hasEvent(time + n)) {
      Event *event = eventqueue.take(time + n);
      activeevents[event->channel].push_back(event);
    }
  }
}

void DrumGizmo::getSamples(Channel *c, sample_t *s, size_t sz)
{
  for(std::list< Event* >::iterator i = activeevents[c->num].begin();
      i != activeevents[c->num].end();
      i++) {
    bool removeevent = false;

    Event *event = *i;

    Event::type_t type = event->type();
    switch(type) {
    case Event::sine:
      {
        EventSine *evt = (EventSine *)event;
        for(size_t n = 0; n < sz; n++) {
          
          if(evt->offset > (time + n)) continue;

          if(evt->t > evt->len) {
            removeevent = true;
            break;
          }

          float x = (float)evt->t / 44100.0;
          float gain = evt->gain;
          gain *= 1.0 - ((float)evt->t / (float)evt->len);
          sample_t val = gain * sin(2.0 * M_PI * evt->freq * x);
          s[n] += val;

          evt->t++;
        }
      }
      break;

    case Event::noise:
      {
        EventNoise *evt = (EventNoise *)event;
        for(size_t n = 0; n < sz; n++) {

          if(evt->offset > (time + n)) continue;

          if(evt->t > evt->len) {
            removeevent = true;
            break;
          }

          float gain = evt->gain;
          gain *= 1.0 - ((float)evt->t / (float)evt->len);
          sample_t val = (float)rand() / (float)RAND_MAX;
          s[n] += val * gain;
          evt->t++;
        }
      }
      break;

    case Event::sample:
      {
        /*
        EventSample *evt = (EventSample *)event;
        AudioFile *af = audiofiles[evt->file];
        af->load(); // Make sure it is loaded.
        for(size_t n = 0; n < sz; n++) {

          if(evt->offset > (time + n)) continue;

          if(evt->t > af->size) {
            removeevent = true;
            break;
          }

          float gain = evt->gain;
          gain *= 1.0 - ((float)evt->t / (float)af->size);
          sample_t val = af->data[evt->t];
          s[n] += val * gain;
          evt->t++;
        }
        */
      }
      break;
    }
    
    if(removeevent) {
      delete event;
      i = activeevents[c->num].erase(i);
      if(activeevents[0].size() +
         activeevents[1].size() +
         eventqueue.size() == 0) {/*is_running = false;*/}
    }
  }
}

void DrumGizmo::post(size_t sz)
{
  time += sz;
}

void DrumGizmo::stop()
{
  // engine.stop();
}

#ifdef TEST_DRUMGIZMO
//deps: instrument.cc sample.cc channel.cc audiofile.cc event.cc
//cflags: $(SNDFILE_CFLAGS)
//libs: $(SNDFILE_LIBS)
#include "test.h"

class AudioOutputEngineDummy : public AudioOutputEngine {
public:
  bool init(Channels *channels) { return true; }
  void run(DrumGizmo *drumgizmo) {}
};

TEST_BEGIN;

AudioOutputEngineDummy a;
DrumGizmo g(a);

Channel ch0("ch0"); g.channels.push_back(ch0);
Channel ch1("ch1"); g.channels.push_back(ch1);

Instrument i("test");
Sample s1; i.addSample(0.0, 1.0, &s1);
Sample s2; i.addSample(0.0, 1.0, &s2);
Sample s3; i.addSample(0.0, 1.0, &s3);

//g.kit.instruments["instr"] = i;

g.run();

TEST_END;

#endif/*TEST_DRUMGIZMO*/

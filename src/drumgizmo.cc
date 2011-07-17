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

#include <event.h>
#include <audiotypes.h>

#include <string.h>

#include "drumkitparser.h"

DrumGizmo::DrumGizmo(AudioOutputEngine *o, AudioInputEngine *i)
  : oe(o), ie(i)
{
}

DrumGizmo::~DrumGizmo()
{
  /*
  AudioFiles::iterator i = audiofiles.begin();
  while(i != audiofiles.end()) {
    AudioFile *audiofile = i->second;
    delete audiofile;
    i++;
  }
  */
}

bool DrumGizmo::loadkit(const std::string &kitfile)
{
  DrumKitParser parser(kitfile, kit);
  if(parser.parse()) return false;
  /*
  Instruments::iterator i = kit.instruments.begin();
  while(i != kit.instruments.end()) {
    Instrument &instr = i->second;
    InstrumentParser iparser(instr.file, instr);
    if(iparser.parse()) return false;
    i++;
  }
  */
  return true;
}

bool DrumGizmo::init(bool preload)
{
  if(preload) {
    /*
    AudioFiles::iterator i = audiofiles.begin();
    while(i != audiofiles.end()) {
      AudioFile *audiofile = i->second;
      audiofile->load();
      i++;
    }
    */
  }

  if(!ie->init(kit.instruments)) return false;
  if(!oe->init(kit.channels)) return false;

  return true;
}

bool DrumGizmo::run(size_t pos, sample_t *samples, size_t nsamples)
{
  ie->pre();
  oe->pre(nsamples);


  //
  // Read new events
  //

  size_t nev;
  event_t *evs = ie->run(pos, nsamples, &nev);

  for(size_t e = 0; e < nev; e++) {
    /*
      printf("Event: type: %d\tinstrument: %d\tvelocity: %f\toffset: %d\n",
      evs[e].type,
      evs[e].instrument,
      evs[e].velocity,
      evs[e].offset);
    */
    if(evs[e].type == TYPE_ONSET) {
      Instrument *i = NULL;
      int d = evs[e].instrument;
      /*
        Instruments::iterator it = kit.instruments.begin();
        while(d-- && it != kit.instruments.end()) {
        i = &(it->second);
        it++;
        }
      */
      
      if(d < (int)kit.instruments.size()) {
        i = &kit.instruments[d];
      }
      
      if(i == NULL) {
        printf("Missing Instrument %d.\n", evs[e].instrument);
        continue;
      }
      
      Sample *s = i->sample(evs[e].velocity);
      
      if(s == NULL) {
        printf("Missing Sample.\n");
        continue;
      }
      
      Channels::iterator j = kit.channels.begin();
      while(j != kit.channels.end()) {
        Channel &ch = *j;
        AudioFile *af = s->getAudioFile(&ch);
        if(af == NULL) {
          //printf("Missing AudioFile.\n");
        } else {
          printf("Adding event %d.\n", evs[e].offset);
          Event *evt = new EventSample(ch.num, 1.0, af);
          evt->offset = evs[e].offset + pos;
          activeevents[ch.num].push_back(evt);
        }
        j++;
      }
    }
    
    if(evs[e].type == TYPE_STOP) {
      return false;
    }
    
  }
    
  free(evs);
  
  
  //
  // Write audio
  //
  
  for(size_t c = 0; c < kit.channels.size(); c++) {
    memset(samples, 0, nsamples * sizeof(sample_t));
    getSamples(c, pos, samples, nsamples);
    oe->run(c, samples, nsamples);
  }
  
  ie->post();
  oe->post(nsamples);
  
  pos += nsamples;

  return true;
}

void DrumGizmo::run()
{
  ie->start();
  oe->start();

  size_t pos = 0;
  size_t nsamples = 512;
  sample_t samples[nsamples];

  bool running = true;

  while(run(pos, samples, nsamples) == true) {
    pos += nsamples;
  }

  ie->stop();
  oe->stop();
}

void DrumGizmo::getSamples(int ch, int pos, sample_t *s, size_t sz)
{
  for(std::list< Event* >::iterator i = activeevents[ch].begin();
      i != activeevents[ch].end();
      i++) {
    bool removeevent = false;

    Event *event = *i;

    Event::type_t type = event->type();
    switch(type) {
    case Event::sample:
      {
        EventSample *evt = (EventSample *)event;
        AudioFile *af = evt->file;
        af->load(); // Make sure it is loaded.
        //        printf("playing: %s (%d)\n", af->filename.c_str(), sz);
        for(size_t n = 0; n < sz; n++) {

          if(evt->offset > (pos + n)) continue;

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
      }
      break;
    }

    if(removeevent) {
      delete event;
      i = activeevents[ch].erase(i);
    }

  }
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

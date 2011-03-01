/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audiooutputenginealsa.cc
 *
 *  Thu Sep 16 11:22:52 CEST 2010
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
#include "audiooutputenginealsa.h"

#include "drumgizmo.h"

#include <string>
#include <math.h>
#include <sys/time.h>

#define T(x, msg) if(x < 0) { printf("%s failed: %s\n", msg, snd_strerror(x)); fflush(stdout); return false; }
#define T_(x, msg) if(x < 0) { printf("%s failed: %s\n", msg, snd_strerror(x)); fflush(stdout); return; }

AudioOutputEngineAlsa::AudioOutputEngineAlsa()
{
  printf("AudioOutputEngineAlsa\n");
	int rc;

  std::string pcmname = "default";

  rc = snd_pcm_open(&handle, pcmname.c_str(),
                    SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
  T_(rc, "snd_pcm_open");
}

AudioOutputEngineAlsa::~AudioOutputEngineAlsa()
{
  printf("~AudioOutputEngineAlsa\n");
  if(handle) snd_pcm_close(handle);
}

bool AudioOutputEngineAlsa::init(Channels *channels)
{
  this->channels = channels;

  if(!handle) {
    printf("No handle!\n");
    return false;
  }

  int rc;
  unsigned int srate = 44100;
  snd_pcm_uframes_t frames = 32;

  // Allocate a hardware parameters object.
  snd_pcm_hw_params_alloca(&params);
  //  if(rc < 0) return false;

  // Fill it in with default values.
  rc = snd_pcm_hw_params_any(handle, params);
  T(rc, "snd_pcm_hw_params_any");

  rc = snd_pcm_hw_params_set_access(handle, params,
                                    SND_PCM_ACCESS_MMAP_NONINTERLEAVED);
  T(rc, "snd_pcm_hw_params_set_access");

  rc = snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_FLOAT);
  T(rc, "snd_pcm_hw_params_set_format");

  rc = snd_pcm_hw_params_set_channels(handle, params, channels->size());
  T(rc, "snd_pcm_hw_params_set_channels");

  rc = snd_pcm_hw_params_set_rate_near(handle, params, &srate, 0);
  T(rc, "snd_pcm_hw_params_set_rate_near");

  rc = snd_pcm_hw_params_set_period_size_near(handle, params, &frames, 0);
  T(rc, "snd_pcm_hw_params_set_period_size_near");

  rc = snd_pcm_hw_params(handle, params);
  T(rc, "snd_pcm_hw_params");

  // SW Params:
  /*
    rc = snd_pcm_sw_params_current(pcm, sw);
    rc = snd_pcm_sw_params_set_start_threshold(pcm, sw, ~0U);
    rc = snd_pcm_sw_params_set_stop_threshold(pcm, sw, ~0U);
    rc = snd_pcm_sw_params(pcm, sw);
    rc = snd_pcm_hw_params_get_buffer_size(hw, &buffer_size);
  */

  channels_t nchannels;
  rc = snd_pcm_hw_params_get_channels(params, &nchannels);
  printf("channels: %d rc: %d\n", nchannels, rc);
  T(rc, "snd_pcm_hw_params_get_channels");
  if(nchannels != channels->size()) return false;

  channel_t c = 0;
  Channels::iterator i = channels->begin();
  while(i != channels->end()) {
    i->num = c;

    i++;
    c++;
  }

  return true;
}

#define MICRO 1000000
void AudioOutputEngineAlsa::run(DrumGizmo *drumgizmo)
{
  int rc;

  snd_pcm_uframes_t nframes;

  while(drumgizmo->isRunning()) {
    timeval tim;
    gettimeofday(&tim, NULL);
    useconds_t t1 = tim.tv_sec * MICRO + tim.tv_usec;

    snd_pcm_uframes_t offset = 0;

    //    unsigned int nchannels = 2;
    //    rc = snd_pcm_hw_params_get_channels(params, &nchannels);
    //    printf("channels: %d rc: %d\n", nchannels, rc);
    //    T_(rc, "snd_pcm_hw_params_get_channels");
    nframes = snd_pcm_avail_update(handle);
    //    printf("%d\n", rc);
    //    T_(rc, "snd_pcm_avail_update");

    //  rc = snd_pcm_hw_params_get_buffer_size(params, &nframes);
    //  T_(rc, "nd_pcm_hw_params_get_buffer_size");
  
    //    nframes = 960;

    const snd_pcm_channel_area_t *areas;
    rc = snd_pcm_mmap_begin(handle, &areas, &offset, &nframes);
    T_(rc, "snd_pcm_mmap_begin");

    drumgizmo->pre((size_t)nframes);

    Channels::iterator i = channels->begin();
    while(i != channels->end()) {
      const snd_pcm_channel_area_t *a = &areas[i->num];
      sample_t *p = (sample_t*)a->addr + offset;
      memset(p, 0, nframes * sizeof(sample_t));
      drumgizmo->getSamples(&(*i), p, (size_t)nframes);

      i++;
    }
    drumgizmo->post((size_t)nframes);

    switch(snd_pcm_state(handle)) {
    case SND_PCM_STATE_PREPARED:
      rc = snd_pcm_mmap_commit(handle, offset, nframes);
      T_(rc, "snd_pcm_mmap_commit");
      rc = snd_pcm_start(handle);
      T_(rc, "snd_pcm_start");
      break;
    case SND_PCM_STATE_RUNNING:
      rc = snd_pcm_mmap_commit(handle, offset, nframes);
      T_(rc, "snd_pcm_mmap_commit");
      break;
    default:
      break;
    }

    useconds_t msec = ((useconds_t)nframes * MICRO)/44100;

    gettimeofday(&tim, NULL);
    useconds_t t2 = tim.tv_sec * MICRO + tim.tv_usec;

    struct timespec slp;
    slp.tv_sec = 0;
    slp.tv_nsec = (msec - (t2 - t1)) * 1000;

    nanosleep(&slp, NULL);

  }
}

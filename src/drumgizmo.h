/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumgizmo.h
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
#ifndef __DRUMGIZMO_DRUMGIZMO_H__
#define __DRUMGIZMO_DRUMGIZMO_H__

#include <string>
#include <list>

#include "audiooutputengine.h"
#include "audioinputengine.h"

#include "events.h"
#include "audiofile.h"
#include "drumkit.h"

#define MAX_NUM_CHANNELS 512

class DrumGizmo {
public:
  DrumGizmo(AudioOutputEngine *outputengine,
            AudioInputEngine *inputengine);
  ~DrumGizmo();

  bool loadkit(const std::string &kitfile);

  bool init(bool preload = true);

  void run();
  void run(size_t pos, sample_t *samples, size_t nsamples);
  void stop();

  void getSamples(int ch, int pos, sample_t *s, size_t sz);

  bool isRunning() { return is_running; }

private:
  bool is_running;
  
  AudioOutputEngine *oe;
  AudioInputEngine *ie;

  std::list< Event* > activeevents[MAX_NUM_CHANNELS];

  std::map<std::string, AudioFile *> audiofiles;

#ifdef TEST_DRUMGIZMO
public:
#endif
  DrumKit kit;
};


#endif/*__DRUMGIZMO_DRUMGIZMO_H__*/
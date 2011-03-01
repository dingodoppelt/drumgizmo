/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audioinputenginemidifile.h
 *
 *  Sun Feb 27 11:43:32 CET 2011
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
#ifndef __DRUMGIZMO_AUDIOINPUTENGINEMIDIFILE_H__
#define __DRUMGIZMO_AUDIOINPUTENGINEMIDIFILE_H__

#include "audioinputengine.h"

class AudioInputEngineMidiFile : public AudioInputEngine {
public:
  AudioInputEngineMidiFile() {}
  ~AudioInputEngineMidiFile() {}

  bool init(EventQueue *eventqueue) { return true; }
  void run(size_t pos, size_t len) {}
};

#endif/*__DRUMGIZMO_AUDIOINPUTENGINEMIDIFILE_H__*/

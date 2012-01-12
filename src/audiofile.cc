/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audiofile.cc
 *
 *  Tue Jul 22 17:14:11 CEST 2008
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
#include "audiofile.h"

#include <stdlib.h>
#include <unistd.h>

#include <sndfile.h>

AudioFile::AudioFile(std::string filename)
{
  //printf("new AudioFile %p\n", this);
  this->filename = filename;

  data = NULL;
  size = 0;

  load();
}

AudioFile::~AudioFile()
{
  //printf("delete AudioFile %p\n", this);
  unload();
}

void AudioFile::unload()
{
  if(data) {
    delete data;
    data = NULL;
    size = 0;
  }
}

void AudioFile::load()
{
  if(data) return;

  SF_INFO sf_info;
  SNDFILE *fh = sf_open(filename.c_str(), SFM_READ, &sf_info);
  if(!fh) {
    printf("SNDFILE Error (%s): %s\n", filename.c_str(), sf_strerror(fh));
    return;
  }
    
  size = sf_info.frames;
  data = new sample_t[size];
  
  sf_read_float(fh, data, size); 
  
  sf_close(fh);
}


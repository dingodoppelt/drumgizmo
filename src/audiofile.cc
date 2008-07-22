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
  this->filename = filename;
  data = NULL;
  size = 0;
  //  load();
}

AudioFile::~AudioFile()
{
  unload();
}

void AudioFile::unload()
{
  if(!data) return;

  free(data);
  data = NULL;
  size = 0;
}

void AudioFile::load()
{
  if(data) return;

  filename = "/tmp/aasimonster/" + filename;

	SF_INFO sf_info;
	SNDFILE *fh = sf_open(filename.c_str(), SFM_READ, &sf_info);

	size = sf_seek(fh, 0, SEEK_END);
	data = (jack_default_audio_sample_t*)malloc(sizeof(jack_default_audio_sample_t)*size);

  //	printf("Loading %s, %d samples\n", filename.c_str(), size);

	sf_seek(fh, 0, SEEK_SET);
	sf_read_float(fh, data, size); 

  //  for(size_t i = 0; i < size; i++) data[i] *= 0.1;

	sf_close(fh);
}


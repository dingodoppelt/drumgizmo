/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            sample.cc
 *
 *  Mon Jul 21 10:23:20 CEST 2008
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
#include "sample.h"

#include <stdlib.h>
#include <unistd.h>

#include <sndfile.h>

Sample::Sample(char *file)
{
	SF_INFO sf_info;
	SNDFILE *fh = sf_open(file, SFM_READ, &sf_info);

	size = sf_seek(fh, 0, SEEK_END);
	data = (jack_default_audio_sample_t*)malloc(sizeof(jack_default_audio_sample_t)*size);

	printf("Loading %s, %d samples\n", file, size);

	sf_seek(fh, 0, SEEK_SET);
	sf_read_float(fh, data, size); 

	sf_close(fh);
}

Sample::~Sample()
{
  free(data);
  data = NULL;
  size = 0;
}

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
 *
 *  Multichannel feature by John Hammen copyright 2014
 */
#include "audiofile.h"

#include <config.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sndfile.h>
#include <hugin.hpp>

#include "configuration.h"

AudioFile::AudioFile(const std::string& filename, int filechannel)
{
	is_loaded = false;
	this->filename = filename;
	this->filechannel = filechannel;

	data = nullptr;
	size = 0;

	magic = this;
}

AudioFile::~AudioFile()
{
	magic = nullptr;
	unload();
}

bool AudioFile::isValid() const
{
	return this == magic;
}

void AudioFile::unload()
{
	// Make sure we don't unload the object while loading it...
	MutexAutolock l(mutex);

	is_loaded = false;

	delete[] data;
	data = nullptr;
	size = 0;
}

#define	BUFFER_SIZE	4092

void AudioFile::load(int num_samples)
{
	// Make sure we don't unload the object while loading it...
	MutexAutolock l(mutex);

	if(data)
	{
		return;
	}

	SF_INFO sf_info{};
	SNDFILE *fh = sf_open(filename.c_str(), SFM_READ, &sf_info);
	if(!fh)
	{
		ERR(audiofile,"SNDFILE Error (%s): %s\n",
		    filename.c_str(), sf_strerror(fh));
		return;
	}

	if(num_samples == ALL_SAMPLES)
	{
		num_samples = sf_info.frames;
	}

	size = sf_info.frames;
	preloadedsize = sf_info.frames;

	if(preloadedsize > (size_t)num_samples)
	{
		preloadedsize = num_samples;
	}

	sample_t* data = new sample_t[preloadedsize];
	if(sf_info.channels == 1)
	{
		preloadedsize = sf_read_float(fh, data, preloadedsize);
	}
	else
	{
		// check filechannel exists
		if(filechannel >= sf_info.channels)
		{
			filechannel = sf_info.channels - 1;
		}

		sample_t buffer[BUFFER_SIZE];
		int readsize = BUFFER_SIZE / sf_info.channels;
		int totalread = 0;
		int read;

		do
		{
	    read = sf_readf_float(fh, buffer, readsize);
	    for(int i = 0; (i < read) && (totalread < num_samples); ++i)
	    {
		    data[totalread++] = buffer[i * sf_info.channels + filechannel];
	    }
		}
		while( (read > 0) &&
		       (totalread < (int)preloadedsize) &&
		       (totalread < num_samples) );

		// set data size to total bytes read
		preloadedsize = totalread;
	}

	sf_close(fh);

	this->data = data;
	is_loaded = true;
}

bool AudioFile::isLoaded() const
{
	return is_loaded;
}

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

#include <hugin.hpp>

#define LAZYLOAD

AudioFile::AudioFile(std::string filename)
{
  is_loaded = false;
  //printf("new AudioFile %p\n", this);
  this->filename = filename;

  locked = false;
  data = NULL;
  size = 0;
//#ifdef LAZY_LOAD  
  fh = NULL;
  preloaded_data = NULL;
  completely_loaded = false;
//#endif/*LAZYLOAD*/
  ref_count = 0;
  magic = this;

  //load();
}

AudioFile::~AudioFile()
{
  magic = NULL;
  //printf("delete AudioFile %p\n", this);
  unload();
}

bool AudioFile::isValid()
{
  return this == magic;
}

void AudioFile::unload()
{
  if(data == preloaded_data) {
    delete data;
    data = NULL;
    size = 0;
  }
  else {
    size = 0;
    delete data;
    data = NULL;
    delete preloaded_data;
    preloaded_data = NULL;
  }
  sf_close(this->fh);
}

#define SIZE 512*4 
void AudioFile::init() {
  printf("Initializing %p\n", this);
  if(data) { 
    printf("\t already initialized\n");
    return;
  }

  SF_INFO sf_info;
  SNDFILE *fh = sf_open(filename.c_str(), SFM_READ, &sf_info);
  if(!fh) {
    printf("SNDFILE Error (%s): %s\n", filename.c_str(), sf_strerror(fh));
    return;
  }
 
  int size = SIZE;

  sample_t* data = new sample_t[size];
  
  size = sf_read_float(fh, data, size); 
  
  printf("Lazy loaded %d samples\n", size);
  sf_close(fh);

  mutex.lock();
  this->data = data;
  this->size = size;
  this->preloaded_data = data;
  this->is_loaded = true;
  this->fh = fh;
//  if(sf_info.frames <= size) {
//    printf("Sample completely loaded\n");
//    completely_loaded = true;
//  }
  mutex.unlock();
}

void AudioFile::loadNext()
{
  if(this->data != this->preloaded_data) {
    printf("Already completely loaded %p\n", this);
    return;
  }

  SF_INFO sf_info;
  SNDFILE *fh = sf_open(filename.c_str(), SFM_READ, &sf_info);
//  SF_INFO sf_info = this->sf_info;
//  SNDFILE *fh = this->fh;
  if(!fh) {
    printf("SNDFILE Error (%s): %s\n", filename.c_str(), sf_strerror(fh));
    return;
  }

//  sf_seek(fh, 0, SEEK_SET) ;
 
  int size = sf_info.frames;

  sample_t* data = new sample_t[size];
  
  size = sf_read_float(fh, data, size); 
  
  printf("Finished loading %d samples %p\n", size, this);
  sf_close(fh);

  mutex.lock();
  this->data = data;
  this->size = size;
  mutex.unlock();
}

void AudioFile::reset() {
  printf("Resetting audio file %p\n", this);
  if(this->data == this->preloaded_data) {
     printf("\tNot completely loaded - skipping %p\n", this);
     return;
  }

  mutex.lock();
  sample_t* old_data = data;
  this->size = SIZE;
  this->data = this->preloaded_data;
//  if() {
    printf("Deleting data %p\n", this);
    delete old_data; 
//  }
  mutex.unlock();
}

void AudioFile::load()
{
#ifdef LAZYLOAD
  init();
  return;
#endif

  if(data) return;

  SF_INFO sf_info;
  SNDFILE *fh = sf_open(filename.c_str(), SFM_READ, &sf_info);
  if(!fh) {
    printf("SNDFILE Error (%s): %s\n", filename.c_str(), sf_strerror(fh));
    return;
  }
 
  size = sf_info.frames;

  data = new sample_t[size];
  
  size = sf_read_float(fh, data, size); 
  
  printf("Loaded %d samples %p\n", size, this);
  
  sf_close(fh);

  mutex.lock();
  is_loaded = true;
  mutex.unlock();

  //DEBUG(audiofile, "Loading of %s completed.\n", filename.c_str());
}

bool AudioFile::isLoaded()
{
  bool l;

  mutex.lock();
  l = is_loaded;
  mutex.unlock();

  return l;
}

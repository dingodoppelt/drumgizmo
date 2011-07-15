/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            mipmap.cc
 *
 *  Fri Sep  3 16:39:46 CEST 2010
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
#include "mipmap.h"

MipMap::MipMap(float *data, size_t size)
{
  this->data = data;
  this->size = size;
  this->zoom = 1;
}

MipMapValue MipMap::data_lookup(size_t begin, size_t end)
{
  MipMapValue val;

  size_t numlavg = 0;
  size_t numuavg = 0;
  for(size_t i = begin; i <= end; i++) {
    if(i > size || i < 0) break;
    if(data[i] > val.max) val.max = data[i];
    if(data[i] < val.min) val.min = data[i];
    
    if(data[i] > 0) { val.uavg += data[i]; numuavg++; }
    if(data[i] < 0) { val.lavg += data[i]; numlavg++; }
  }
  
  if(numlavg) val.lavg /= (float) numlavg;
  if(numuavg) val.uavg /= (float) numuavg;

  return val;
}

MipMapValue MipMap::mipmap_lookup(size_t begin, size_t end)
{
  MipMapValue val;

  size_t numlavg = 0;
  size_t numuavg = 0;
  for(size_t i = begin; i <= end; i++) {
    if(i > size || i < 0) break;
    if(data[i] > val.max) val.max = data[i];
    if(data[i] < val.min) val.min = data[i];
    
    if(data[i] > 0) { val.uavg += data[i]; numuavg++; }
    if(data[i] < 0) { val.lavg += data[i]; numlavg++; }
  }
  
  if(numlavg) val.lavg /= (float) numlavg;
  if(numuavg) val.uavg /= (float) numuavg;

  return val;
}

#define ABS(x) (x>0?x:-x)

MipMapValue MipMap::lookup(size_t begin, size_t end)
{
  return data_lookup(begin, end);
  /*

  size_t zoom_factor = ABS(end - begin);

  if(zoom_factor < zoom / 2) {
    if(zoom == 1) { // Lookup in original data.
      return data_lookup(begin, end);
    }

    return mipmap_lookup(begin, end);
  }

  if(lowerlevel) return lowerlevel->lookup(begin,end);

  return MipMapValue();
  */
}

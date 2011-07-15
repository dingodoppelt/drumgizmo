/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            mipmap.h
 *
 *  Fri Sep  3 16:39:45 CEST 2010
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
#ifndef __DRUMGIZMO_MIPMAP_H__
#define __DRUMGIZMO_MIPMAP_H__

#include <QMap>
#include <stddef.h>

class MipMapValue {
public:
  MipMapValue() { max = min = uavg = lavg = 0.0; }
  float max;
  float min;
  float uavg;
  float lavg;
};

class MipMap {
public:
  MipMap(float *data, size_t size);

  MipMapValue lookup(size_t begin, size_t end);

private:
  float *data;
  size_t size;

  MipMapValue *values;

  size_t zoom;

  MipMapValue data_lookup(size_t begin, size_t end);
  MipMapValue mipmap_lookup(size_t begin, size_t end);

  MipMap *lowerlevel;
};

#endif/*__DRUMGIZMO_MIPMAP_H__*/

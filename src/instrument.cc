/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            instrument.cc
 *
 *  Tue Jul 22 17:14:20 CEST 2008
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
#include "instrument.h"

#include <stdlib.h>

Instrument::Instrument(std::string name)
{
  this->name = name;
}

Sample *Instrument::sample(level_t level)
{
  std::vector<Sample*> s = samples.get(level);
  if(s.size() == 0) return NULL;
  size_t idx = rand()%(s.size());
  return s[idx];
}

void Instrument::addSample(level_t a, level_t b, Sample *s)
{
  samples.insert(a, b, s);
}

#ifdef TEST_INSTRUMENT
//deps: channel.cc sample.cc audiofile.cc
//cflags: $(SNDFILE_CFLAGS)
//libs: $(SNDFILE_LIBS)
#include "test.h"

TEST_BEGIN;

Instrument i("test");

Sample *a = new Sample();
i.addSample(0.0, 1.0, a);

Sample *b = new Sample();
i.addSample(0.0, 1.0, b);

Sample *c = new Sample();
i.addSample(1.5, 1.7, c);

TEST_EQUAL(i.sample(0.0), b, "?");
TEST_EQUAL(i.sample(0.0), a, "?");
TEST_EQUAL(i.sample(0.0), b, "?");
TEST_EQUAL(i.sample(0.0), b, "?");
TEST_EQUAL(i.sample(0.0), b, "?");
TEST_EQUAL(i.sample(0.0), b, "?");
TEST_EQUAL(i.sample(0.0), a, "?");
TEST_EQUAL(i.sample(0.0), a, "?");

TEST_EQUAL(i.sample(2.0), NULL, "?");

TEST_EQUAL(i.sample(1.6), c, "?");
TEST_EQUAL(i.sample(1.6), c, "?");
TEST_EQUAL(i.sample(1.6), c, "?");

TEST_END;

#endif/*TEST_INSTRUMENT*/

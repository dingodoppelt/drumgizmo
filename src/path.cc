/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            path.cc
 *
 *  Tue May  3 14:42:47 CEST 2011
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
#include "path.h"

#include <libgen.h>
#include <string.h>
#include <stdlib.h>

std::string getPath(std::string file)
{
  char *b = strdup(file.c_str());
  std::string p = dirname(b);
  free(b);
  return p;
}

#ifdef TEST_PATH
//Additional dependency files
//deps:
//Required cflags (autoconf vars may be used)
//cflags:
//Required link options (autoconf vars may be used)
//libs:
#include "test.h"

TEST_BEGIN;

std::string a = "../dir/file";
TEST_EQUAL_STR(getPath(a), "../dir", "relative path");

std::string b = "/abs/path/file";
TEST_EQUAL_STR(getPath(b), "/abs/path", "absolute path");


TEST_END;

#endif/*TEST_PATH*/

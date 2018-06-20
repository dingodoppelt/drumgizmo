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
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "path.h"

#ifndef __MINGW32__
#include <libgen.h>
#endif/*__MINGW32__*/

#include <string.h>
#include <stdlib.h>

std::string getPath(const std::string& file)
{
	std::string path;

#ifdef __MINGW32__
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	_splitpath(file.c_str(), drive, dir, nullptr, nullptr);
	path = std::string(drive) + dir;
#else
	// POSIX
	char* buffer = strdup(file.c_str());
	path = dirname(buffer);
	free(buffer);
#endif

	return path;
}

std::string getFile(const std::string& file)
{
	std::string path;

#ifdef __MINGW32__
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath(file.c_str(), nullptr, nullptr, fname, ext);
	path = std::string(fname) + "." + ext;
#else
	// POSIX
	char* buffer = strdup(file.c_str());
	path = basename(buffer);
	free(buffer);
#endif

	return path;
}

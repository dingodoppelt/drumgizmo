/* -*- Mode: c++ -*- */
/***************************************************************************
 *            scopedfile.cc
 *
 *  Wed Jun  6 15:15:31 CEST 2018
 *  Copyright 2018 Bent Bisballe Nyeng
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
#include "scopedfile.h"

#include <cstdlib>
#include <unistd.h>

#include <cpp11fix.h>

struct Pimpl
{
	std::string filename;
	int fd;
};

ScopedFile::ScopedFile(const std::string& data)
	: pimpl(std::make_unique<struct Pimpl>())
{
	char templ[] = "/tmp/dg-scoped-file-XXXXXX"; // buffer for filename
	pimpl->fd = mkstemp(templ);
	pimpl->filename = templ;
	write(pimpl->fd, data.data(), data.size());
	close(pimpl->fd);
}

ScopedFile::~ScopedFile()
{
	unlink(pimpl->filename.data());
}

std::string ScopedFile::filename() const
{
	return pimpl->filename;
}

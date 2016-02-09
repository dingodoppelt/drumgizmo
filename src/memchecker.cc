/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            memchecker.cc
 *
 *  Sat Jan 16 18:27:52 CET 2016
 *  Copyright 2016 André Nusser
 *  andre.nusser@googlemail.com
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
#include "memchecker.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/sysinfo.h>
#endif
#include <sndfile.h>
#include <hugin.hpp>

bool MemChecker::enoughFreeMemory(const DrumKit& drumkit) const
{
	uint64_t free_memory = calcFreeMemory();
	uint64_t needed_memory = calcNeededMemory(drumkit);

	return free_memory >= needed_memory;
}

uint64_t MemChecker::calcFreeMemory() const
{
	uint64_t free_memory = 0;

	// Platform specific calculation of the amount of free memory.
#ifdef WIN32
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	free_memory = status.ullAvailPhys;
#else
	struct sysinfo sys_info;
	sysinfo(&sys_info);
	free_memory = sys_info.freeram * sys_info.mem_unit;
#endif

	DEBUG(memchecker, "Calculated %" PRIu64 " free memory.\n", free_memory);

	return free_memory;
}

uint64_t MemChecker::calcNeededMemory(const DrumKit& drumkit) const
{
	uint64_t needed_memory = 0;

	// Calculate memory usage of all instruments of drumkit.
	for(auto instrument : drumkit.instruments)
	{
		const auto& audiofiles = instrument->audiofiles;

		// Calculate memory usage of all audiofiles.
		for(auto audiofile : audiofiles)
		{
			needed_memory += calcBytesPerChannel(audiofile->filename);
		}
	}

	DEBUG(memchecker, "Calculated %" PRIu64 " needed memory.\n", needed_memory);

	return needed_memory;
}

uint64_t MemChecker::calcBytesPerChannel(const std::string& filename) const
{
	SF_INFO sf_info{};

	SNDFILE* f = sf_open(filename.c_str(), SFM_READ, &sf_info);
	if(!f)
	{
		ERR(memchecker, "SNDFILE Error (%s): %s\n", filename.c_str(), sf_strerror(f));
		return 0;
	}

	sf_close(f);

	return sf_info.frames * sizeof(sample_t);
}

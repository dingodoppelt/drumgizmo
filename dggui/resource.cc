/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            resource.cc
 *
 *  Sun Mar 17 19:38:04 CET 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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
#include "resource.h"

#include <hugin.hpp>
#include <cstdio>
#include <climits>

#include <platform.h>

#if DG_PLATFORM != DG_PLATFORM_WINDOWS
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

// rcgen generated file containing rc_data declaration.
#include "resource_data.h"

extern const rc_data_t* rc_data;

namespace GUI
{

// TODO: Replace with std::filesystem::is_regular_file once we update the
// compiler to require C++17
static bool pathIsFile(const std::string& path)
{
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
	return (GetFileAttributesA(path.data()) & FILE_ATTRIBUTE_DIRECTORY) == 0;
#else
	struct stat s;
	if(stat(path.data(), &s) != 0)
	{
		return false; // error
	}

	return (s.st_mode & S_IFREG) != 0; // s.st_mode & S_IFDIR => dir
#endif
}

// Internal resources start with a colon.
static bool nameIsInternal(const std::string& name)
{
	return name.size() && (name[0] == ':');
}

Resource::Resource(const std::string& name)
{
	isValid = false;

	if(nameIsInternal(name))
	{
		// Use internal resource:

		// Find internal resource in rc_data.
		const rc_data_t* p = rc_data;
		while(p && *p->name) // last entry in rc_data has the name := ""
		{
			if(name == p->name)
			{
				internalData = p->data;
				internalSize = p->size;
				break;
			}
			++p;
		}

		// We did not find the named resource.
		if(internalData == nullptr)
		{
			ERR(rc, "Could not find '%s'\n", name.c_str());
			return;
		}

		isInternal = true;
	}
	else
	{
		if(!pathIsFile(name))
		{
			return;
		}

		// Read from file:
		std::FILE *fp = std::fopen(name.data(), "rb");
		if(!fp)
		{
			return;
		}

		// Get the file size
		if(std::fseek(fp, 0, SEEK_END) == -1)
		{
			std::fclose(fp);
			return;
		}

		long filesize = std::ftell(fp);

		// Apparently fseek doesn't fail if fp points to a directory that has been
		// opened (which doesn't fail either!!) and ftell will then fail by either
		// returning -1 or LONG_MAX
		if(filesize == -1L || filesize == LONG_MAX)
		{
			std::fclose(fp);
			return;
		}

		// Reserve space in the string for the data.
		externalData.reserve(filesize);

		// Rewind and read...
		std::rewind(fp);

		char buffer[32];
		while(!std::feof(fp))
		{
			size_t size = std::fread(buffer, 1, sizeof(buffer), fp);
			externalData.append(buffer, size);
		}

		std::fclose(fp);

		isInternal = false;
	}

	isValid = true;
}

const char *Resource::data()
{
	if(isValid == false)
	{
		return nullptr;
	}

	if(isInternal)
	{
		return internalData;
	}
	else
	{
		return externalData.data();
	}
}

size_t Resource::size()
{
	if(isValid == false)
	{
	  return 0;
	}

	if(isInternal)
	{
		return internalSize;
	}
	else
	{
		return externalData.length();
	}
}

bool Resource::valid()
{
	return isValid;
}

} // GUI::

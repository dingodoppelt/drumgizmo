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
#include "resource.h"

#include <hugin.hpp>
#include <cstdio>

// rcgen generated file containing rc_data declaration.
#include "resource_data.h"

namespace GUI {

// Internal resources start with a colon.
static bool nameIsInternal(const std::string& name)
{
	return name.size() && (name[0] == ':');
}

Resource::Resource(const std::string& name)
{
	if(nameIsInternal(name))
	{
		// Use internal resource:

		// Find internal resource in rc_data.
		const rc_data_t* p = rc_data;
		while(p->name) // last entry in rc_data has the name := ""
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
		// Read from file:
		std::FILE *fp = std::fopen(name.c_str(), "rb");
		if(!fp)
		{
			return;
		}

		// Get the file size
		std::fseek(fp, 0, SEEK_END);
		size_t filesize = ftell(fp);

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

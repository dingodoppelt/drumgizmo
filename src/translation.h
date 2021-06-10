/* -*- Mode: c++ -*- */
/***************************************************************************
 *            translation.h
 *
 *  Sun Sep  8 14:13:22 CEST 2019
 *  Copyright 2019 Bent Bisballe Nyeng
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
#pragma once

#include <cstdint>

#include <config.h>

#ifdef WITH_NLS

#include <memory>

std::uint64_t constexpr const_hash(const char* input)
{
	return *input ?
		static_cast<unsigned int>(*input) + 33 * const_hash(input + 1) :
		5381;
}

#define _(msg) Translation::gettext(const_hash(msg), msg)

class Translation
{
public:
	Translation();
	virtual ~Translation();

	bool load(const char* catalog, std::size_t size);

	static const char* gettext(std::uint64_t msgid, const char* original);
	static std::string getISO639LanguageName();
};

#else

#define _(msg) msg

#endif

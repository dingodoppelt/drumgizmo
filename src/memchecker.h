/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            memchecker.h
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
 *  the Free Software Foundation; either version 3 of the License, or
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
#pragma once

#include "drumkit.h"

#include <string>
// includes cstdint automatically and is needed for the PRIu64 macro
#include <cinttypes>

class MemChecker
{
public:
	//! Checks if there is enough memory left to load drumkit into RAM.
	//! \param drumkit The drumkit for which it is checked if there's enough memory left.
	//! \return True iff there is enough memory left.
	bool enoughFreeMemory(const DrumKit& drumkit) const;

protected:
	// Computes how much RAM (in bytes) is left.
	uint64_t calcFreeMemory() const;

	// Computes how much memory the drumkit takes when loaded into RAM (in bytes).
	uint64_t calcNeededMemory(const DrumKit& drumkit) const;

	// Computes the number of bytes per channel of <filename> using libsnd.
	uint64_t calcBytesPerChannel(const std::string& filename) const;
};

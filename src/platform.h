/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            platform.h
 *
 *  Fri May 20 18:46:17 CEST 2016
 *  Copyright 2016 Bent Bisballe Nyeng
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

//! The DG platform types:
#define DG_PLATFORM_LINUX 1   //!< Platform is Linux based.
#define DG_PLATFORM_WINDOWS 2 //!< Platform is Windows based
#define DG_PLATFORM_OSX 3     //!< Platform is MacOSX based.
#define DG_PLATFORM_FREEBSD 4 //!< Platform is FreeBSD based.
#define DG_PLATFORM_UNIX 5    //!< Platform is Unix based.

#ifdef __linux__
	#define DG_PLATFORM DG_PLATFORM_LINUX

#elif _WIN32
	#define DG_PLATFORM DG_PLATFORM_WINDOWS

	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>

#elif __APPLE__
	#define DG_PLATFORM DG_PLATFORM_OSX

#elif __FreeBSD__
	#define DG_PLATFORM DG_PLATFORM_FREEBSD

#elif __unix__
	// All other unices (*BSD etc)
	#define DG_PLATFORM DG_PLATFORM_UNIX

#endif

#ifndef DG_PLATFORM
	#error "Platform not defined!"
#endif

#if defined(_POSIX_VERSION)
	// POSIX
	#define DG_POSIX
#endif

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            enginefactory.h
 *
 *  Mi 20. Jan 10:46:07 CET 2016
 *  Copyright 2016 Christian Glöckner
 *  cgloeckner@freenet.de
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
#include <list>
#include <string>
#include <memory>

#include "audioinputengine.h"
#include "audiooutputengine.h"

#if defined(HAVE_INPUT_JACKMIDI) || defined(HAVE_OUTPUT_JACKAUDIO)
#define USE_JACK
#include "jackclient.h"
#endif

#ifdef HAVE_INPUT_DUMMY
#include "input/inputdummy.h"
#endif

#ifdef HAVE_INPUT_TEST
#include "input/test.h"
#endif

#ifdef HAVE_INPUT_MIDIFILE
#include "input/midifile.h"
#endif

#ifdef HAVE_INPUT_JACKMIDI
#include "input/jackmidi.h"
#endif

#ifdef HAVE_INPUT_ALSAMIDI
#include "input/alsamidi.h"
#endif

#ifdef HAVE_INPUT_OSS
#include "input/ossmidi.h"
#endif

#ifdef HAVE_OUTPUT_DUMMY
#include "output/outputdummy.h"
#endif

#ifdef HAVE_OUTPUT_WAVFILE
#include "output/wavfile.h"
#endif

#ifdef HAVE_OUTPUT_ALSA
#include "output/alsa.h"
#endif

#ifdef HAVE_OUTPUT_JACKAUDIO
#include "output/jackaudio.h"
#endif

#ifdef HAVE_OUTPUT_OSS
#include "output/oss.h"
#endif

#ifdef HAVE_INPUT_OSSMIDI
#include "input/ossmidi.h"
#endif

#ifdef HAVE_INPUT_ALSAMIDI
#include "input/alsamidi.h"
#endif


//! Factory for various input- and output engines
class EngineFactory
{
public:
	//! Initialized lists of available engines (depending on enabled settings)
	EngineFactory();

	//! Query list of available input engines' names
	//! \return const reference to the list
	const std::list<std::string>& getInputEngines() const;

	//! Query list of available output engines' names
	//! \return const reference to the list
	const std::list<std::string>& getOutputEngines() const;

	//! Create input engine by name
	//! \param name Name of the input engine that should be initialized
	//! \return unique_ptr to input engine or nullptr if engine is not supported
	std::unique_ptr<AudioInputEngine> createInput(const std::string& name);

	//! Create input engine by name
	//! \param name Name of the output engine that should be initialized
	//! \return unique_ptr to input engine or nullptr if engine is not supported
	std::unique_ptr<AudioOutputEngine> createOutput(const std::string& name);

protected:
	std::list<std::string> input, output; // available engines

#ifdef USE_JACK
	// Client that is used by all jack-related engines
	std::unique_ptr<JackClient> jack;

	// Utility to initialize jack (once)
	void prepareJack();
#endif
};

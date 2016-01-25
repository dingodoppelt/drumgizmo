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

#ifdef HAVE_INPUT_MIDIFILE
#include "input/midifile.h"
#endif

#ifdef HAVE_INPUT_JACKMIDI
#include "input/jackmidi.h"
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

class EngineFactory
{
private:
	std::list<std::string> input, output; // available engines

#ifdef USE_JACK
	std::unique_ptr<JackClient> jack;

	void prepareJack();
#endif

public:
	EngineFactory();

	std::list<std::string> const &getInputEngines() const;
	std::list<std::string> const &getOutputEngines() const;

	std::unique_ptr<AudioInputEngine> createInput(std::string const &name);
	std::unique_ptr<AudioOutputEngine> createOutput(std::string const &name);
};

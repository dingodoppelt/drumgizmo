/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            enginefactory.cc
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
#include <iostream>

#include "enginefactory.h"
#include "jackclient.h"

#ifdef HAVE_INPUT_DUMMY
#include "input/inputdummy.h"
#endif

#ifdef HAVE_INPUT_MIDIFILE
#include "input/midifile.h"
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

InputEnginePtr createInputEngine(std::string const & name) {
#ifdef HAVE_INPUT_DUMMY
	if (name == "dummy") {
		return std::make_unique<DummyInputEngine>();
	}
#endif
#ifdef HAVE_INPUT_MIDIFILE
	if (name == "midifile") {
		return std::make_unique<MidifileInputEngine>();
	}
#endif
	
	// todo: add more engines
	
	std::cerr << "Unsupported input engine '" << name << "'\n";
	return nullptr;
}

OutputEnginePtr createOutputEngine(std::string const & name) {
#ifdef HAVE_OUTPUT_DUMMY
	if (name == "dummy") {
		return std::make_unique<DummyOutputEngine>();
	}
#endif
#ifdef HAVE_OUTPUT_WAVFILE
	if (name == "wavfile") {
		return std::make_unique<WavfileOutputEngine>();
	}
#endif
#ifdef HAVE_OUTPUT_ALSA
	if (name == "alsa") {
		return std::make_unique<AlsaOutputEngine>();
	}
#endif
	
	// todo: add more engines
	
	std::cerr << "Unsupported output engine '" << name << "'\n";
	return nullptr;
}

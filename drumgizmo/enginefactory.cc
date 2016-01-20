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
#include "enginefactory.h"
#include "jackclient.h"
#include "input/midifile.h"
#include "output/wavfile.h"

InputEnginePtr createInputEngine(std::string const & name) {
	if (name == "midifile") {
		return std::make_unique<MidifileInputEngine>();
	}
	// todo: add more engines
	
	printf("Unsupported input engine: %s\n", name.c_str());
	return nullptr;
}

OutputEnginePtr createOutputEngine(std::string const & name) {
	if (name == "wavfile") {
		return std::make_unique<WavfileOutputEngine>();
	}
	// todo: add more engines
	
	printf("Unsupported output engine: %s\n", name.c_str());
	return nullptr;
}

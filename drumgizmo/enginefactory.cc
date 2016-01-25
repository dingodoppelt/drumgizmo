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

#include "cpp11fix.h" // required for c++11
#include "enginefactory.h"

EngineFactory::EngineFactory()
	: input{}
	, output{}
#ifdef USE_JACK
	, jack { nullptr }
#endif
{
// list available input engines
#ifdef HAVE_INPUT_DUMMY
	input.push_back("inputdummy");
#endif
#ifdef HAVE_INPUT_MIDIFILE
	input.push_back("midifile");
#endif
#ifdef HAVE_INPUT_JACKMIDI
	input.push_back("jackmidi");
#endif

// list available output engines
#ifdef HAVE_OUTPUT_DUMMY
	output.push_back("outputdummy");
#endif
#ifdef HAVE_OUTPUT_WAVFILE
	output.push_back("wavfile");
#endif
#ifdef HAVE_OUTPUT_ALSA
	output.push_back("alsa");
#endif
#ifdef HAVE_OUTPUT_JACKAUDIO
	output.push_back("jackaudio");
#endif
}

#ifdef USE_JACK
void EngineFactory::prepareJack()
{
	if (jack == nullptr)
	{
		jack = std::make_unique<JackClient>();
	}
}
#endif

const std::list<std::string>& EngineFactory::getInputEngines() const
{
	return input;
}

const std::list<std::string>& EngineFactory::getOutputEngines() const
{
	return output;
}

std::unique_ptr<AudioInputEngine>
EngineFactory::createInput(const std::string& name)
{
#ifdef HAVE_INPUT_DUMMY
	if (name == "dummy")
	{
		return std::make_unique<DummyInputEngine>();
	}
#endif
#ifdef HAVE_INPUT_MIDIFILE
	if (name == "midifile")
	{
		return std::make_unique<MidifileInputEngine>();
	}
#endif
#ifdef HAVE_INPUT_JACKMIDI
	if (name == "jackmidi")
	{
		prepareJack();
		return std::make_unique<JackMidiInputEngine>(*jack);
	}
#endif

	// todo: add more engines

	std::cerr << "[EngineFactory] Unsupported input engine '" << name << "'\n";
	return nullptr;
}

std::unique_ptr<AudioOutputEngine>
EngineFactory::createOutput(const std::string& name)
{
#ifdef HAVE_OUTPUT_DUMMY
	if (name == "dummy")
	{
		return std::make_unique<DummyOutputEngine>();
	}
#endif
#ifdef HAVE_OUTPUT_WAVFILE
	if (name == "wavfile")
	{
		return std::make_unique<WavfileOutputEngine>();
	}
#endif
#ifdef HAVE_OUTPUT_ALSA
	if (name == "alsa")
	{
		return std::make_unique<AlsaOutputEngine>();
	}
#endif
#ifdef HAVE_OUTPUT_JACKAUDIO
	if (name == "jackaudio")
	{
		prepareJack();
		return std::make_unique<JackAudioOutputEngine>(*jack);
	}
#endif

	// todo: add more engines

	std::cerr << "[EngineFactory] Unsupported output engine '" << name << "'\n";
	return nullptr;
}

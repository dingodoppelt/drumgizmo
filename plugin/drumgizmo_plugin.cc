/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumgizmo_lv2.cc
 *
 *  Wed Mar  2 17:31:32 CET 2016
 *  Copyright 2016 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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
#include "drumgizmo_plugin.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <iostream>

#include <midievent.h>

#ifdef LV2
// Entry point for lv2 plugin instantiation.
PluginLV2* createEffectInstance()
{
	return new DrumGizmoPlugin();
}
#endif

#ifdef VST
// Entry point for vst plugin instantiation.
AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
	return new DrumGizmoPlugin(audioMaster);
}

DrumGizmoPlugin::DrumGizmoPlugin(audioMasterCallback audioMaster)
	: PluginVST(audioMaster)
#else
DrumGizmoPlugin::DrumGizmoPlugin()
#endif
{
	init();

	drumgizmo = std::make_shared<DrumGizmo>(settings, output, input);
	resizeWindow(370, 330);
	drumgizmo->setFreeWheel(true);
	drumgizmo->setSamplerate(44100);
	drumgizmo->setFrameSize(2048);
}

void DrumGizmoPlugin::onFreeWheelChange(bool freewheel)
{
	drumgizmo->setFreeWheel(freewheel);
}

void DrumGizmoPlugin::onSamplerateChange(float samplerate)
{
	drumgizmo->setSamplerate(samplerate);
}

void DrumGizmoPlugin::onFramesizeChange(size_t framesize)
{
	drumgizmo->setFrameSize(framesize);
}

void DrumGizmoPlugin::onActiveChange(bool active)
{
}

std::string DrumGizmoPlugin::onStateSave()
{
	return drumgizmo->configString();
}

void DrumGizmoPlugin::onStateRestore(const std::string& config)
{
	drumgizmo->setConfigString(config);
}

size_t DrumGizmoPlugin::getNumberOfMidiInputs()
{
	return 1;
}

size_t DrumGizmoPlugin::getNumberOfMidiOutputs()
{
	return 0;
}

size_t DrumGizmoPlugin::getNumberOfAudioInputs()
{
	return 0;
}

size_t DrumGizmoPlugin::getNumberOfAudioOutputs()
{
	return 16;
}

std::string DrumGizmoPlugin::getId()
{
	return "DrumGizmo";
}
static float g_samples[16*  4096];
void DrumGizmoPlugin::process(size_t pos,
                              const std::vector<MidiEvent>& input_events,
                              std::vector<MidiEvent>& output_events,
                              const std::vector<const float*>& input_samples,
                              const std::vector<float*>& output_samples,
                              size_t count)
{
	setLatency(drumgizmo->getLatency());

	this->input_events = &input_events;
	this->output_samples = &output_samples;

	drumgizmo->run(pos, g_samples, count);

	this->input_events = nullptr;
	this->output_samples = nullptr;
}

bool DrumGizmoPlugin::hasGUI()
{
	return true;
}

void DrumGizmoPlugin::createWindow(void *parent)
{
	plugin_gui = std::make_shared<GUI::PluginGUI>(settings, parent);
	resizeWindow(370, 330);
	onShowWindow();
}

void DrumGizmoPlugin::onDestroyWindow()
{
	plugin_gui = nullptr;
}

void DrumGizmoPlugin::onShowWindow()
{
	plugin_gui->show();
}

void DrumGizmoPlugin::onHideWindow()
{
	plugin_gui->hide();
}

void DrumGizmoPlugin::onIdle()
{
	plugin_gui->processEvents();
}

void DrumGizmoPlugin::closeWindow()
{
}


//
// Input Engine
//

DrumGizmoPlugin::Input::Input(DrumGizmoPlugin& plugin)
	: plugin(plugin)
{
}

bool DrumGizmoPlugin::Input::init(const Instruments &instruments)
{
	this->instruments = &instruments;
	return true;
}

void DrumGizmoPlugin::Input::setParm(const std::string& parm, const std::string& value)
{
}

bool DrumGizmoPlugin::Input::start()
{
	return true;
}

void DrumGizmoPlugin::Input::stop()
{
}

void DrumGizmoPlugin::Input::pre()
{
}

void DrumGizmoPlugin::Input::run(size_t pos, size_t len, std::vector<event_t>& events)
{
	assert(events.empty());
	assert(plugin.input_events);

	events.reserve(plugin.input_events->size());

	for(auto& event : *plugin.input_events)
	{
		if(event.type !=  MidiEventType::NoteOn)
		{
			continue;
		}

		int i = mmap.lookup(event.key);
		if(event.velocity && (i != -1))
		{
			events.push_back({TYPE_ONSET, (size_t)i, (size_t)event.getTime(), event.velocity / 127.0f});
		}
	}
}

void DrumGizmoPlugin::Input::post()
{
}


//
// Output Engine
//

DrumGizmoPlugin::Output::Output(DrumGizmoPlugin& plugin)
	: plugin(plugin)
{
}

bool DrumGizmoPlugin::Output::init(const Channels& channels)
{
	return true;
}

void DrumGizmoPlugin::Output::setParm(const std::string& parm, const std::string& value)
{
}

bool DrumGizmoPlugin::Output::start()
{
	return true;
}

void DrumGizmoPlugin::Output::stop()
{
}

void DrumGizmoPlugin::Output::pre(size_t nsamples)
{
}

void DrumGizmoPlugin::Output::run(int ch, sample_t *samples, size_t nsamples)
{
	assert(plugin.output_samples);

	assert(sizeof(float) == sizeof(sample_t));

	if((std::size_t)ch >= plugin.output_samples->size())
	{
		return;
	}

	if((*plugin.output_samples)[ch] == nullptr)
	{
		// Port not connected.
		return;
	}

	// We are not running directly on the internal buffer: do a copy.
	if((*plugin.output_samples)[ch] != samples)
	{
		memcpy((*plugin.output_samples)[ch], samples, nsamples * sizeof(sample_t));
	}
}

void DrumGizmoPlugin::Output::post(size_t nsamples)
{
}

sample_t* DrumGizmoPlugin::Output::getBuffer(int ch) const
{
	assert(plugin.output_samples);

	if((std::size_t)ch >= plugin.output_samples->size())
	{
		return nullptr;
	}

	return (*plugin.output_samples)[ch];
}

size_t DrumGizmoPlugin::Output::getBufferSize() const
{
	return plugin.getFramesize();
}

bool DrumGizmoPlugin::Output::isFreewheeling() const
{
	return plugin.getFreeWheel();
}

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumgizmo_lv2.h
 *
 *  Wed Mar  2 17:31:31 CET 2016
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
#pragma once

#include <memory>

#if defined(X11)
#include <X11/Xlib.h>
#endif // defined(X11)

#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // defined(WIN32)

#ifdef LV2
#include <pluginlv2.h>
#endif

#ifdef VST
#include <pluginvst.h>
#endif

#include <drumgizmo.h>
#include <audioinputenginemidi.h>
#include <audiooutputengine.h>
#include <plugingui.h>

class DrumGizmoPlugin
#ifdef LV2
	: public PluginLV2
#endif
#ifdef VST
	: public PluginVST
#endif
{
public:
#ifdef VST
	DrumGizmoPlugin(audioMasterCallback audioMaster);
#else
	DrumGizmoPlugin();
#endif

	void onFreeWheelChange(bool freewheel) override;

	void onFramesizeChange(size_t framesize) override;

	void onSamplerateChange(float samplerate) override;

	void onActiveChange(bool active) override;

	std::string onStateSave() override;
	void onStateRestore(const std::string& config) override;

	size_t getNumberOfMidiInputs() override;
	size_t getNumberOfMidiOutputs() override;
	size_t getNumberOfAudioInputs() override;
	size_t getNumberOfAudioOutputs() override;

	std::string getId() override;

	void process(size_t pos,
	             const std::vector<MidiEvent>& input_events,
	             std::vector<MidiEvent>& output_events,
	             const std::vector<const float*>& input_samples,
	             const std::vector<float*>& output_samples,
	             size_t count) override;

	//
	// GUI
	//
	bool hasGUI() override;
	void createWindow(void *parent) override;
	void onDestroyWindow() override;
	void onShowWindow() override;
	void onHideWindow() override;
	void onIdle() override;
	void closeWindow() override;

private:
	class Input
		: public AudioInputEngineMidi
	{
	public:
		Input(DrumGizmoPlugin& plugin);

		bool init(const Instruments& instruments) override;

		void setParm(const std::string& parm, const std::string& value) override;

		bool start() override;
		void stop() override;

		void pre() override;
		void run(size_t pos, size_t len, std::vector<event_t>& events) override;
		void post() override;

	protected:
		DrumGizmoPlugin& plugin;
		const Instruments* instruments{nullptr};
	};

	class Output
		: public AudioOutputEngine
	{
	public:
		Output(DrumGizmoPlugin& plugin);

		bool init(const Channels& channels) override;

		void setParm(const std::string& parm, const std::string& value) override;

		bool start() override;
		void stop() override;

		void pre(size_t nsamples) override;
		void run(int ch, sample_t *samples, size_t nsamples) override;
		void post(size_t nsamples) override;

		sample_t *getBuffer(int ch) override;
		size_t getBufferSize() override;

	protected:
		DrumGizmoPlugin& plugin;
	};

	Input input{*this};
	const std::vector<MidiEvent>* input_events{nullptr};

	Output output{*this};
	const std::vector<float*>* output_samples{nullptr};

	std::shared_ptr<GUI::PluginGUI> plugin_gui;
	std::shared_ptr<DrumGizmo> drumgizmo;
};

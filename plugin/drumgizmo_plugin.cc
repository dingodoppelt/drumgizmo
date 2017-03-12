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

#include <cstring>
#include <algorithm>

#include <midievent.h>

#include <hugin.hpp>

#include "configparser.h"
#include "nolocale.h"

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
	: PluginVST(audioMaster),
#else
DrumGizmoPlugin::DrumGizmoPlugin()
	:
#endif
	config_string_io(settings)
{
	init();

	drumgizmo = std::make_shared<DrumGizmo>(settings, output, input);
	resizeWindow(370, 330);
	drumgizmo->setFreeWheel(true);
	drumgizmo->setSamplerate(44100);
	drumgizmo->setFrameSize(2048);
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
	return config_string_io.get();
}

void DrumGizmoPlugin::onStateRestore(const std::string& config)
{
	config_string_io.set(config);
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

std::string DrumGizmoPlugin::getEffectName()
{
	return "DrumGizmo";
}

std::string DrumGizmoPlugin::getVendorString()
{
	return "DrumGizmo Team";
}

std::string DrumGizmoPlugin::getProductString()
{
	return "DrumGizmo";
}

PluginCategory DrumGizmoPlugin::getPluginCategory()
{
	return PluginCategory::Synth;
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

bool DrumGizmoPlugin::hasInlineGUI()
{
	return true;
}

class InlinePixelBufferAlpha
	: public GUI::PixelBufferAlpha
{
public:
	InlinePixelBufferAlpha(Plugin::InlineDrawContext& context)
	{
		buf = context.data;
		width = context.width;
		height = context.height;
		x = 0;
		y = 0;
	}
};

class InlineCanvas
	: public GUI::Canvas
{
public:
	InlineCanvas(Plugin::InlineDrawContext& context)
		: pixbuf(context)
	{
	}

	// From Canvas:
	GUI::PixelBufferAlpha& GetPixelBuffer() override
	{
		return pixbuf;
	}

private:
	InlinePixelBufferAlpha pixbuf;
};

void DrumGizmoPlugin::onInlineRedraw(std::size_t width,
                                     std::size_t max_height,
                                     InlineDrawContext& context)
{
	std::size_t height = std::min(std::size_t(11), max_height);
	if(!context.data ||
	   (context.width != width) ||
	   (context.height != height) ||
	   settingsGetter.number_of_files.hasChanged() ||
	   settingsGetter.number_of_files_loaded.hasChanged() ||
	   settingsGetter.drumkit_load_status.hasChanged())
	{
		context.width = width;
		context.height = height;
		assert(context.width * context.height <= sizeof(inlineDisplayBuffer));

		context.data = (unsigned char*)inlineDisplayBuffer;
		box.setSize(context.width, context.height);

		InlineCanvas canvas(context);
		GUI::Painter painter(canvas);
		painter.clear();
		painter.drawImage(0, 0, box);

		double progress =
			(double)settingsGetter.number_of_files_loaded.getValue() /
			(double)settingsGetter.number_of_files.getValue();

		int brd = 4;
		int val = (width - (2 * brd)) * progress;

		switch(settingsGetter.drumkit_load_status.getValue())
		{
		case LoadStatus::Error:
			bar_red.setSize(val, height);
			painter.drawImage(brd, 0, bar_red);
			break;
		case LoadStatus::Done:
			bar_green.setSize(val, height);
			painter.drawImage(brd, 0, bar_green);
			break;
		case LoadStatus::Loading:
		case LoadStatus::Idle:
			bar_blue.setSize(val, height);
			painter.drawImage(brd, 0, bar_blue);
			break;
		}

		// Convert to correct pixel format
		for(std::size_t i = 0; i < context.height * context.width; ++i)
		{
			std::uint32_t pixel = inlineDisplayBuffer[i];
			unsigned char* p = (unsigned char*)&pixel;
			inlineDisplayBuffer[i] = pgzRGBA(p[0], p[1], p[2], p[3]);
		}
	}
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

bool DrumGizmoPlugin::Input::isFreewheeling() const
{
	return plugin.getFreeWheel();
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
	// Clear all channels
	for(auto& channel : *plugin.output_samples)
	{
		if(channel)
		{
			std::memset(channel, 0, nsamples * sizeof(sample_t));
		}
	}
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

//
// ConfigStringIO
//

// anonymous namespace for helper furnctions of ConfigStringIO
namespace
{

std::string float2str(float a)
{
	char buf[256];
	snprintf_nol(buf, sizeof(buf) - 1, "%f", a);
	return buf;
}

std::string bool2str(bool a)
{
	return a?"true":"false";
}

float str2float(std::string a)
{
	if(a == "")
	{
		return 0.0;
	}

	return atof_nol(a.c_str());
}

} // end anonymous namespace

DrumGizmoPlugin::ConfigStringIO::ConfigStringIO(Settings& settings)
	: settings(settings)
{

}

std::string DrumGizmoPlugin::ConfigStringIO::get()
{
	return
		"<config>\n"
		"  <value name=\"drumkitfile\">" + settings.drumkit_file.load() + "</value>\n"
		"  <value name=\"midimapfile\">" + settings.midimap_file.load() + "</value>\n"
		"  <value name=\"enable_velocity_modifier\">" +
		bool2str(settings.enable_velocity_modifier.load()) + "</value>\n"
		"  <value name=\"velocity_modifier_falloff\">" +
		float2str(settings.velocity_modifier_falloff.load()) + "</value>\n"
		"  <value name=\"velocity_modifier_weight\">" +
		float2str(settings.velocity_modifier_weight.load()) + "</value>\n"
		"  <value name=\"enable_velocity_randomiser\">" +
		bool2str(settings.enable_velocity_randomiser.load()) + "</value>\n"
		"  <value name=\"velocity_randomiser_weight\">" +
		float2str(settings.velocity_randomiser_weight.load()) + "</value>\n"
		"  <value name=\"disk_cache_upper_limit\">" +
		float2str(settings.disk_cache_upper_limit.load()) + "</value>\n"
		"  <value name=\"disk_cache_enable\">" +
		bool2str(settings.disk_cache_enable.load()) + "</value>\n"
		"</config>";
}

bool DrumGizmoPlugin::ConfigStringIO::set(std::string config_string)
{
	DEBUG(config, "Load config: %s\n", config_string.c_str());

	ConfigParser p;
	if(p.parseString(config_string))
	{
		ERR(config, "Config parse error.\n");
		return false;
	}

	if(p.value("enable_velocity_modifier") != "")
	{
		settings.enable_velocity_modifier.store(p.value("enable_velocity_modifier") == "true");
	}

	if(p.value("velocity_modifier_falloff") != "")
	{
		settings.velocity_modifier_falloff.store(str2float(p.value("velocity_modifier_falloff")));
	}

	if(p.value("velocity_modifier_weight") != "")
	{
		settings.velocity_modifier_weight.store(str2float(p.value("velocity_modifier_weight")));
	}

	if(p.value("enable_velocity_randomiser") != "")
	{
		settings.enable_velocity_randomiser.store(p.value("enable_velocity_randomiser") == "true");
	}

	if(p.value("velocity_randomiser_weight") != "")
	{
		settings.velocity_randomiser_weight.store(str2float(p.value("velocity_randomiser_weight")));
	}

	if(p.value("enable_resampling") != "")
	{
		settings.enable_resampling.store(p.value("enable_resampling") == "true");
	}

	if(p.value("disk_cache_upper_limit") != "")
	{
		settings.disk_cache_upper_limit.store(str2float(p.value("disk_cache_upper_limit")));
	}

	if(p.value("disk_cache_enable") != "")
	{
		settings.disk_cache_enable.store(p.value("disk_cache_enable") == "true");
	}

	std::string newkit = p.value("drumkitfile");
	if(newkit != "")
	{
		settings.drumkit_file.store(newkit);
	}

	std::string newmidimap = p.value("midimapfile");
	if(newmidimap != "")
	{
		settings.midimap_file.store(newmidimap);
	}

	return true;
}

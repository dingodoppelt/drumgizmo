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

#include <config.h>

#include <cstring>
#include <algorithm>
#include <string>

#include <midievent.h>

#include <hugin.hpp>

#include "configparser.h"
#include "nolocale.h"

#ifdef LV2
// Entry point for lv2 plugin instantiation.
PG_EXPORT PluginLV2* createEffectInstance()
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
	resizeWindow(width, height);
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
	return NUM_CHANNELS;
}

std::string DrumGizmoPlugin::getId()
{
	return "DrumGizmo";
}

std::string DrumGizmoPlugin::getURI()
{
	return "http://drumgizmo.org";
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

std::string DrumGizmoPlugin::getHomepage()
{
	return "https://www.drumgizmo.org";
}

PluginCategory DrumGizmoPlugin::getPluginCategory()
{
	return PluginCategory::Synth;
}

static float g_samples[NUM_CHANNELS *  4096];

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
	GUI::PixelBufferAlpha& getPixelBuffer() override
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
	std::size_t bar_height = bar_red.height();
	std::size_t image_height = ((double)width / inline_display_image.width()) * inline_display_image.height();

	bool show_bar{false};
	bool show_image{false};
	std::size_t height = 0;

	if(bar_height <= max_height)
	{
		show_bar = true;
		height += bar_height;
	}
	if(bar_height + image_height <= max_height)
	{
		show_image = true;
		height += image_height;
	}

	// They have to be called seperately to avoid lazy evaluation
	bool nof_changed = settingsGetter.number_of_files.hasChanged();
	bool nofl_changed = settingsGetter.number_of_files_loaded.hasChanged();
	bool dls_changed = settingsGetter.drumkit_load_status.hasChanged();

	bool in_progress =
		settingsGetter.number_of_files_loaded.getValue() <
		settingsGetter.number_of_files.getValue();

	bool context_needs_update =
	    !context.data || context.width != width || context.height != height;
	bool bar_needs_update =
		nof_changed || nofl_changed || dls_changed || context_needs_update |
		in_progress; // Always update while loading to prevent flickering.
	bool image_needs_update = inline_image_first_draw || context_needs_update;
	// TODO: settingsGetter.inline_image_filename.hasChanged();
	bool something_needs_update =
	    context_needs_update || bar_needs_update || image_needs_update;

	if (something_needs_update)
	{
		context.width = width;
		context.height = height;
		assert(context.width * context.height <= sizeof(inlineDisplayBuffer));

		context.data = (unsigned char*)inlineDisplayBuffer;
		InlineCanvas canvas(context);
		GUI::Painter painter(canvas);

		double progress =
			(double)settingsGetter.number_of_files_loaded.getValue() /
			(double)settingsGetter.number_of_files.getValue();

		if(show_bar && bar_needs_update)
		{
			box.setSize(context.width, bar_height);
			painter.drawImage(0, height - bar_height, box);

			int brd = 4;
			int val = (width - (2 * brd)) * progress;

			switch(settingsGetter.drumkit_load_status.getValue())
			{
			case LoadStatus::Error:
				bar_red.setSize(val, bar_height);
				painter.drawImage(brd, height - bar_height, bar_red);
				break;
			case LoadStatus::Done:
				bar_green.setSize(val, bar_height);
				painter.drawImage(brd, height - bar_height, bar_green);
				break;
			case LoadStatus::Loading:
			case LoadStatus::Idle:
				bar_blue.setSize(val, bar_height);
				painter.drawImage(brd, height - bar_height, bar_blue);
				break;
			}
		}

		if(show_image && image_needs_update)
		{
			// TODO: load new image and remove the bool
			inline_image_first_draw = false;

			painter.setColour(.5);
			painter.drawFilledRectangle(0, 0, width, image_height);
			painter.drawImageStretched(0, 0, inline_display_image, width, image_height);
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

void* DrumGizmoPlugin::createWindow(void* parent)
{
	plugin_gui = std::make_shared<GUI::MainWindow>(settings, parent);
	resizeWindow(width, height);
	onShowWindow();

	return plugin_gui->getNativeWindowHandle();
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
		processNote((const std::uint8_t*)event.getData(), event.getSize(),
		            event.getTime(), events);
	}
}

void DrumGizmoPlugin::Input::post()
{
}

bool DrumGizmoPlugin::Input::isFreewheeling() const
{
	return plugin.getFreeWheel();
}

bool DrumGizmoPlugin::Input::loadMidiMap(const std::string& file,
                                         const Instruments& i)
{
	bool result = AudioInputEngineMidi::loadMidiMap(file, i);
	std::vector<std::pair<int, std::string>> midnam;

	const auto& map = mmap.getMap();
	midnam.reserve(map.size());
	for(const auto& m : map)
	{
		midnam.push_back(std::make_pair(m.first, m.second));
	}

	if(midnam.size() > 0)
	{
		plugin.setMidnamData(midnam);
	}
	return result;
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

void DrumGizmoPlugin::Output::run(int ch, sample_t* samples, size_t nsamples)
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

std::size_t DrumGizmoPlugin::Output::getSamplerate() const
{
	return plugin.drumgizmo->samplerate();
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

std::string int2str(int a)
{
	char buf[256];
	snprintf(buf, sizeof(buf) - 1, "%d", a);
	return buf;
}

float str2float(std::string a)
{
	if(a == "")
	{
		return 0.0;
	}

	return atof_nol(a.c_str());
}

int str2int(std::string a)
{
	try
	{
		return std::stoi(a);
	}
	catch(...)
	{
		return 0;
	}
}

long long str2ll(std::string a)
{
	try
	{
		return std::stoll(a);
	}
	catch(...)
	{
		return 0;
	}
}

} // end anonymous namespace

DrumGizmoPlugin::ConfigStringIO::ConfigStringIO(Settings& settings)
	: settings(settings)
{

}

std::string DrumGizmoPlugin::ConfigStringIO::get()
{
	return
		"<config version=\"1.0\">\n"
		"  <value name=\"drumkitfile\">" + settings.drumkit_file.load() + "</value>\n"
		"  <value name=\"midimapfile\">" + settings.midimap_file.load() + "</value>\n"
		"  <value name=\"enable_velocity_modifier\">" +
		bool2str(settings.enable_velocity_modifier.load()) + "</value>\n"
		"  <value name=\"velocity_modifier_falloff\">" +
		float2str(settings.velocity_modifier_falloff.load()) + "</value>\n"
		"  <value name=\"velocity_modifier_weight\">" +
		float2str(settings.velocity_modifier_weight.load()) + "</value>\n"
		"  <value name=\"velocity_stddev\">" +
		float2str(settings.velocity_stddev.load()) + "</value>\n"
		"  <value name=\"sample_selection_f_close\">" +
		float2str(settings.sample_selection_f_close.load()) + "</value>\n"
		"  <value name=\"sample_selection_f_diverse\">" +
		float2str(settings.sample_selection_f_diverse.load()) + "</value>\n"
		"  <value name=\"sample_selection_f_random\">" +
		float2str(settings.sample_selection_f_random.load()) + "</value>\n"
		"  <value name=\"enable_velocity_randomiser\">" +
		bool2str(settings.enable_velocity_randomiser.load()) + "</value>\n"
		"  <value name=\"velocity_randomiser_weight\">" +
		float2str(settings.velocity_randomiser_weight.load()) + "</value>\n"
		"  <value name=\"enable_resampling\">" +
		bool2str(settings.enable_resampling.load()) + "</value>\n"
		"  <value name=\"disk_cache_upper_limit\">" +
		int2str(settings.disk_cache_upper_limit.load()) + "</value>\n"
		"  <value name=\"disk_cache_chunk_size\">" +
		int2str(settings.disk_cache_chunk_size.load()) + "</value>\n"
		"  <value name=\"disk_cache_enable\">" +
		bool2str(settings.disk_cache_enable.load()) + "</value>\n"
		"  <value name=\"enable_bleed_control\">" +
		bool2str(settings.enable_bleed_control.load()) + "</value>\n"
		"  <value name=\"master_bleed\">" +
		float2str(settings.master_bleed.load()) + "</value>\n"
		"  <value name=\"enable_latency_modifier\">" +
		bool2str(settings.enable_latency_modifier.load()) + "</value>\n"
		// Do not store/reload this value
		//"  <value name=\"latency_max\">" +
		//int2str(settings.latency_max.load()) + "</value>\n"
		"  <value name=\"latency_laid_back_ms\">" +
		float2str(settings.latency_laid_back_ms.load()) + "</value>\n"
		"  <value name=\"latency_stddev\">" +
		float2str(settings.latency_stddev.load()) + "</value>\n"
		"  <value name=\"latency_regain\">" +
		float2str(settings.latency_regain.load()) + "</value>\n"
		"</config>";
}

bool DrumGizmoPlugin::ConfigStringIO::set(std::string config_string)
{
	DEBUG(config, "Load config: %s\n", config_string.c_str());

	ConfigParser p;
	if(!p.parseString(config_string))
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

	if(p.value("velocity_stddev") != "")
	{
		settings.velocity_stddev.store(str2float(p.value("velocity_stddev")));
	}

	if(p.value("sample_selection_f_close") != "")
	{
		settings.sample_selection_f_close.store(str2float(p.value("sample_selection_f_close")));
	}

	if(p.value("sample_selection_f_diverse") != "")
	{
		settings.sample_selection_f_diverse.store(str2float(p.value("sample_selection_f_diverse")));
	}

	if(p.value("sample_selection_f_random") != "")
	{
		settings.sample_selection_f_random.store(str2float(p.value("sample_selection_f_random")));
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
		settings.disk_cache_upper_limit.store(str2ll(p.value("disk_cache_upper_limit")));
	}

	if(p.value("disk_cache_chunk_size") != "")
	{
		settings.disk_cache_chunk_size.store(str2int(p.value("disk_cache_chunk_size")));
	}

	if(p.value("disk_cache_enable") != "")
	{
		settings.disk_cache_enable.store(p.value("disk_cache_enable") == "true");
	}

	if(p.value("enable_bleed_control") != "")
	{
		settings.enable_bleed_control.store(p.value("enable_bleed_control") == "true");
	}

	if(p.value("master_bleed") != "")
	{
		settings.master_bleed.store(str2float(p.value("master_bleed")));
	}

	if(p.value("enable_latency_modifier") != "")
	{
		settings.enable_latency_modifier.store(p.value("enable_latency_modifier") == "true");
	}

	// Do not store/reload this value
	//if(p.value("latency_max") != "")
	//{
	//	settings.latency_max.store(str2int(p.value("latency_max")));
	//}

	if(p.value("latency_laid_back_ms") != "")
	{
		settings.latency_laid_back_ms.store(str2float(p.value("latency_laid_back_ms")));
	}

	if(p.value("latency_stddev") != "")
	{
		settings.latency_stddev.store(str2float(p.value("latency_stddev")));
	}

	if(p.value("latency_regain") != "")
	{
		settings.latency_regain.store(str2float(p.value("latency_regain")));
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

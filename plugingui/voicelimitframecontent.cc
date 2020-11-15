/***************************************************************************
 *            voicelimitframecontent.cc
 *
 *  Wed Aug 26 14:53:03 CEST 2020
 *  Copyright 2020 The Marlboro Man
 *  marlborometal@gmail.com
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
#include "voicelimitframecontent.h"

#include <settings.h>

namespace GUI
{

VoiceLimitFrameContent::VoiceLimitFrameContent(Widget* parent,
                                               Settings& settings,
                                               SettingsNotifier& settings_notifier)
	: Widget(parent)
	, settings(settings)
	, settings_notifier(settings_notifier)
{
	//Setup frame.
	label_text.setText(_("Per-instrument voice limit:"));
	label_text.setAlignment(TextAlignment::center);

	//Setup layout
	layout.setResizeChildren(false);

	auto setup_control =
		[](Knob& knob,
		   LabeledControl& label,
		   GridLayout& layout,
		   const GridLayout::GridRange& gridrange,
		   float min,
		   float max,
		   float defaultval)
		{
			knob.resize(30, 30);
			knob.showValue(false);
			knob.setDefaultValue(defaultval);
			knob.setRange(min, max);
			label.resize(80, 80);
			label.setControl(&knob);
			layout.addItem(&label);
			layout.setPosition(&label, gridrange);
	};

	setup_control(knob_max_voices, lc_max_voices, layout, {0, 1, 0, 1},
	              1.0f, 30.0f, Settings::voice_limit_max_default);

	setup_control(knob_rampdown_time, lc_rampdown_time, layout, {1, 2, 0, 1},
	              0.01f, 2.0f, Settings::voice_limit_rampdown_default);


	auto voices_transform =
		[this](double new_value, double scale, double offset) -> std::string
		{
			new_value *= scale;
			new_value += offset;
			return std::to_string(convertMaxVoices(new_value));
		};

	lc_max_voices.setValueTransformationFunction(voices_transform);

	//GUI to settings.
	CONNECT(&knob_max_voices, valueChangedNotifier,
	        this, &VoiceLimitFrameContent::maxvoicesKnobValueChanged);

	CONNECT(&knob_rampdown_time, valueChangedNotifier,
	        this, &VoiceLimitFrameContent::rampdownKnobValueChanged);

	//Settings to GUI
	CONNECT(this, settings_notifier.voice_limit_max,
	        this, &VoiceLimitFrameContent::maxvoicesSettingsValueChanged);

	CONNECT(this, settings_notifier.voice_limit_rampdown,
	        this, &VoiceLimitFrameContent::rampdownSettingsValueChanged);
}

void VoiceLimitFrameContent::maxvoicesKnobValueChanged(float value)
{
	settings.voice_limit_max.store((int)value);
}

void VoiceLimitFrameContent::rampdownKnobValueChanged(float value)
{
	settings.voice_limit_rampdown.store(value);
}

void VoiceLimitFrameContent::maxvoicesSettingsValueChanged(float value)
{
	knob_max_voices.setValue(convertMaxVoices(value));
}

void VoiceLimitFrameContent::rampdownSettingsValueChanged(float value)
{
	knob_rampdown_time.setValue(value);
}

std::size_t VoiceLimitFrameContent::convertMaxVoices(float value)
{
	return static_cast<std::size_t>(value);
}

}

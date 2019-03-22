/* -*- Mode: c++ -*- */
/***************************************************************************
 *            humanizerframecontent.cc
 *
 *  Fri Mar 24 21:49:58 CET 2017
 *  Copyright 2017 André Nusser
 *  andre.nusser@googlemail.com
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
#include "humanizerframecontent.h"

#include <settings.h>

#include "painter.h"

namespace GUI
{

HumanizerframeContent::HumanizerframeContent(Widget* parent,
                                             Settings& settings,
                                             SettingsNotifier& settings_notifier)
	: Widget(parent)
	, settings(settings)
	, settings_notifier(settings_notifier)
{
	layout.setResizeChildren(false);

	attack.resize(80, 80);
	attack_knob.resize(30, 30);
	attack_knob.showValue(false);
	attack_knob.setDefaultValue(Settings::velocity_modifier_weight_default);
	attack.setControl(&attack_knob);
	layout.addItem(&attack);

	falloff.resize(80, 80);
	falloff_knob.resize(30, 30);
	falloff_knob.showValue(false);
	falloff_knob.setDefaultValue(Settings::velocity_modifier_falloff_default);
	falloff.setControl(&falloff_knob);
	layout.addItem(&falloff);

	stddev.resize(80, 80);
	stddev_knob.resize(30, 30);
	stddev_knob.showValue(false);
	stddev_knob.setDefaultValue(Settings::velocity_stddev_default/stddev_factor);
	stddev.setControl(&stddev_knob);
	layout.addItem(&stddev);

	f_distance.resize(80, 80);
	f_distance_knob.resize(30, 30);
	f_distance_knob.showValue(false);
	f_distance_knob.setDefaultValue(Settings::sample_selection_f_distance_default/f_distance_factor);
	f_distance.setControl(&f_distance_knob);
	layout.addItem(&f_distance);

	f_recent.resize(80, 80);
	f_recent_knob.resize(30, 30);
	f_recent_knob.showValue(false);
	f_recent_knob.setDefaultValue(Settings::sample_selection_f_recent_default/f_recent_factor);
	f_recent.setControl(&f_recent_knob);
	layout.addItem(&f_recent);

	f_random.resize(80, 80);
	f_random_knob.resize(30, 30);
	f_random_knob.showValue(false);
	f_random_knob.setDefaultValue(Settings::sample_selection_f_random_default/f_random_factor);
	f_random.setControl(&f_random_knob);
	layout.addItem(&f_random);

	layout.setPosition(&attack, GridLayout::GridRange{0, 1, 0, 1});
	layout.setPosition(&falloff, GridLayout::GridRange{1, 2, 0, 1});
	layout.setPosition(&stddev, GridLayout::GridRange{2, 3, 0, 1});
	layout.setPosition(&f_distance, GridLayout::GridRange{0, 1, 1, 2});
	layout.setPosition(&f_recent, GridLayout::GridRange{1, 2, 1, 2});
	layout.setPosition(&f_random, GridLayout::GridRange{2, 3, 1, 2});

	CONNECT(this, settings_notifier.velocity_modifier_weight,
	        &attack_knob, &Knob::setValue);
	CONNECT(this, settings_notifier.velocity_modifier_falloff,
	        &falloff_knob, &Knob::setValue);
	CONNECT(this, settings_notifier.velocity_stddev,
	        this, &HumanizerframeContent::stddevSettingsValueChanged);
	CONNECT(this, settings_notifier.sample_selection_f_distance,
	        this, &HumanizerframeContent::fDistanceSettingsValueChanged);
	CONNECT(this, settings_notifier.sample_selection_f_recent,
	        this, &HumanizerframeContent::fRecentSettingsValueChanged);
	CONNECT(this, settings_notifier.sample_selection_f_random,
	        this, &HumanizerframeContent::fRandomSettingsValueChanged);

	CONNECT(&attack_knob, valueChangedNotifier,
	        this, &HumanizerframeContent::attackValueChanged);
	CONNECT(&falloff_knob, valueChangedNotifier,
	        this, &HumanizerframeContent::falloffValueChanged);
	CONNECT(&stddev_knob, valueChangedNotifier,
	        this, &HumanizerframeContent::stddevKnobValueChanged);
	CONNECT(&f_distance_knob, valueChangedNotifier,
	        this, &HumanizerframeContent::fDistanceKnobValueChanged);
	CONNECT(&f_recent_knob, valueChangedNotifier,
	        this, &HumanizerframeContent::fRecentKnobValueChanged);
	CONNECT(&f_random_knob, valueChangedNotifier,
	        this, &HumanizerframeContent::fRandomKnobValueChanged);
}

void HumanizerframeContent::attackValueChanged(float value)
{
	settings.velocity_modifier_weight.store(value);
}

void HumanizerframeContent::falloffValueChanged(float value)
{
	settings.velocity_modifier_falloff.store(value);
}

void HumanizerframeContent::stddevKnobValueChanged(float value)
{
	settings.velocity_stddev.store(value*stddev_factor);
}

void HumanizerframeContent::fDistanceKnobValueChanged(float value)
{
	settings.sample_selection_f_distance.store(value*f_distance_factor);
}

void HumanizerframeContent::fRecentKnobValueChanged(float value)
{
	settings.sample_selection_f_recent.store(value*f_recent_factor);
}

void HumanizerframeContent::fRandomKnobValueChanged(float value)
{
	settings.sample_selection_f_random.store(value*f_random_factor);
}

void HumanizerframeContent::stddevSettingsValueChanged(float value)
{
	stddev_knob.setValue(value/stddev_factor);
}

void HumanizerframeContent::fDistanceSettingsValueChanged(float value)
{
	f_distance_knob.setValue(value/f_distance_factor);
}

void HumanizerframeContent::fRecentSettingsValueChanged(float value)
{
	f_recent_knob.setValue(value/f_recent_factor);
}

void HumanizerframeContent::fRandomSettingsValueChanged(float value)
{
	f_random_knob.setValue(value/f_random_factor);
}

} // GUI::

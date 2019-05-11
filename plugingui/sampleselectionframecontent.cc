/* -*- Mode: c++ -*- */
/***************************************************************************
 *            sampleselectionframecontent.cc
 *
 *  Sat May 11 15:29:25 CEST 2019
 *  Copyright 2019 André Nusser
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
#include "sampleselectionframecontent.h"

#include <settings.h>

#include "painter.h"

namespace GUI
{

SampleselectionframeContent::SampleselectionframeContent(Widget* parent,
                                             Settings& settings,
                                             SettingsNotifier& settings_notifier)
	: Widget(parent)
	, settings(settings)
	, settings_notifier(settings_notifier)
{
	layout.setResizeChildren(false);

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

	layout.setPosition(&f_distance, GridLayout::GridRange{0, 1, 0, 1});
	layout.setPosition(&f_recent, GridLayout::GridRange{1, 2, 0, 1});
	layout.setPosition(&f_random, GridLayout::GridRange{2, 3, 0, 1});

	CONNECT(this, settings_notifier.sample_selection_f_distance,
	        this, &SampleselectionframeContent::fDistanceSettingsValueChanged);
	CONNECT(this, settings_notifier.sample_selection_f_recent,
	        this, &SampleselectionframeContent::fRecentSettingsValueChanged);
	CONNECT(this, settings_notifier.sample_selection_f_random,
	        this, &SampleselectionframeContent::fRandomSettingsValueChanged);

	CONNECT(&f_distance_knob, valueChangedNotifier,
	        this, &SampleselectionframeContent::fDistanceKnobValueChanged);
	CONNECT(&f_recent_knob, valueChangedNotifier,
	        this, &SampleselectionframeContent::fRecentKnobValueChanged);
	CONNECT(&f_random_knob, valueChangedNotifier,
	        this, &SampleselectionframeContent::fRandomKnobValueChanged);
}

void SampleselectionframeContent::fDistanceKnobValueChanged(float value)
{
	settings.sample_selection_f_distance.store(value*f_distance_factor);
}

void SampleselectionframeContent::fRecentKnobValueChanged(float value)
{
	settings.sample_selection_f_recent.store(value*f_recent_factor);
}

void SampleselectionframeContent::fRandomKnobValueChanged(float value)
{
	settings.sample_selection_f_random.store(value*f_random_factor);
}

void SampleselectionframeContent::fDistanceSettingsValueChanged(float value)
{
	f_distance_knob.setValue(value/f_distance_factor);
}

void SampleselectionframeContent::fRecentSettingsValueChanged(float value)
{
	f_recent_knob.setValue(value/f_recent_factor);
}

void SampleselectionframeContent::fRandomSettingsValueChanged(float value)
{
	f_random_knob.setValue(value/f_random_factor);
}

} // GUI::

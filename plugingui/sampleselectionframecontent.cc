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

	f_close.resize(80, 80);
	f_close_knob.resize(30, 30);
	f_close_knob.showValue(false);
	f_close_knob.setDefaultValue(Settings::sample_selection_f_close_default);
	f_close.setControl(&f_close_knob);
	layout.addItem(&f_close);

	f_diverse.resize(80, 80);
	f_diverse_knob.resize(30, 30);
	f_diverse_knob.showValue(false);
	f_diverse_knob.setDefaultValue(Settings::sample_selection_f_diverse_default);
	f_diverse.setControl(&f_diverse_knob);
	layout.addItem(&f_diverse);

	f_random.resize(80, 80);
	f_random_knob.resize(30, 30);
	f_random_knob.showValue(false);
	f_random_knob.setDefaultValue(Settings::sample_selection_f_random_default);
	f_random.setControl(&f_random_knob);
	layout.addItem(&f_random);

	layout.setPosition(&f_close, GridLayout::GridRange{0, 1, 0, 1});
	layout.setPosition(&f_diverse, GridLayout::GridRange{1, 2, 0, 1});
	layout.setPosition(&f_random, GridLayout::GridRange{2, 3, 0, 1});

	CONNECT(this, settings_notifier.sample_selection_f_close,
	        this, &SampleselectionframeContent::fCloseSettingsValueChanged);
	CONNECT(this, settings_notifier.sample_selection_f_diverse,
	        this, &SampleselectionframeContent::fDiverseSettingsValueChanged);
	CONNECT(this, settings_notifier.sample_selection_f_random,
	        this, &SampleselectionframeContent::fRandomSettingsValueChanged);

	CONNECT(&f_close_knob, valueChangedNotifier,
	        this, &SampleselectionframeContent::fCloseKnobValueChanged);
	CONNECT(&f_diverse_knob, valueChangedNotifier,
	        this, &SampleselectionframeContent::fDiverseKnobValueChanged);
	CONNECT(&f_random_knob, valueChangedNotifier,
	        this, &SampleselectionframeContent::fRandomKnobValueChanged);
}

void SampleselectionframeContent::fCloseKnobValueChanged(float value)
{
	settings.sample_selection_f_close.store(value);
}

void SampleselectionframeContent::fDiverseKnobValueChanged(float value)
{
	settings.sample_selection_f_diverse.store(value);
}

void SampleselectionframeContent::fRandomKnobValueChanged(float value)
{
	settings.sample_selection_f_random.store(value);
}

void SampleselectionframeContent::fCloseSettingsValueChanged(float value)
{
	f_close_knob.setValue(value);
}

void SampleselectionframeContent::fDiverseSettingsValueChanged(float value)
{
	f_diverse_knob.setValue(value);
}

void SampleselectionframeContent::fRandomSettingsValueChanged(float value)
{
	f_random_knob.setValue(value);
}

} // GUI::

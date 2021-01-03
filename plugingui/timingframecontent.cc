/* -*- Mode: c++ -*- */
/***************************************************************************
 *            timingframecontent.cc
 *
 *  Sat Oct 14 19:39:33 CEST 2017
 *  Copyright 2017 Bent Bisballe Nyeng
 *  deva@aasimon.org
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
#include "timingframecontent.h"

#include <cmath>
#include <iostream>

#include <dggui/painter.h>

namespace GUI
{

TimingframeContent::TimingframeContent(dggui::Widget* parent,
                                       Settings& settings,
                                       SettingsNotifier& settings_notifier)
	: dggui::Widget(parent)
	, settings(settings)
	, settings_notifier(settings_notifier)
{
	layout.setResizeChildren(false);

	tightness.resize(80, 80);
	tightness_knob.resize(30, 30);
	tightness_knob.showValue(false);
	tightness_knob.setDefaultValue(tightnessSettingsToKnob(Settings::latency_stddev_default));
	tightness.setControl(&tightness_knob);
	layout.addItem(&tightness);

	regain.resize(80, 80);
	regain_knob.resize(30, 30);
	regain_knob.showValue(false);
	regain_knob.setDefaultValue(Settings::latency_regain_default);
	regain.setControl(&regain_knob);
	layout.addItem(&regain);

	laidback.resize(80, 80);
	laidback_knob.resize(30, 30);
	laidback_knob.showValue(false);
	laidback_knob.setDefaultValue(Settings::latency_laid_back_ms_default);
	laidback_knob.setRange(-100.0f, 100.0f); // +/- 100 ms range
	laidback.setControl(&laidback_knob);
	layout.addItem(&laidback);

	layout.setPosition(&tightness, dggui::GridLayout::GridRange{0, 1, 0, 1});
	layout.setPosition(&regain, dggui::GridLayout::GridRange{1, 2, 0, 1});
	layout.setPosition(&laidback, dggui::GridLayout::GridRange{2, 3, 0, 1});

	CONNECT(this, settings_notifier.latency_stddev,
	        this, &TimingframeContent::tightnessSettingsValueChanged);
	CONNECT(this, settings_notifier.latency_regain,
	        this, &TimingframeContent::regainSettingsValueChanged);
	CONNECT(this, settings_notifier.latency_laid_back_ms,
	        this, &TimingframeContent::laidbackSettingsValueChanged);

	CONNECT(&tightness_knob, valueChangedNotifier,
	        this, &TimingframeContent::tightnessKnobValueChanged);
	CONNECT(&regain_knob, valueChangedNotifier,
	        this, &TimingframeContent::regainKnobValueChanged);
	CONNECT(&laidback_knob, valueChangedNotifier,
	        this, &TimingframeContent::laidbackKnobValueChanged);
}

float TimingframeContent::thightnessKnobToSettings(float value) const
{
	return (1.f - value) * 20.f;
}

float TimingframeContent::tightnessSettingsToKnob(float value) const
{
	return 1.f - (value / 20.f);
}

void TimingframeContent::tightnessKnobValueChanged(float value)
{
	auto settings_value = thightnessKnobToSettings(value);
	settings.latency_stddev.store(settings_value);
}

void TimingframeContent::tightnessSettingsValueChanged(float value)
{
	auto knob_value = tightnessSettingsToKnob(value);
	tightness_knob.setValue(knob_value);
}

void TimingframeContent::regainKnobValueChanged(float value)
{
	settings.latency_regain.store(value);
}

void TimingframeContent::regainSettingsValueChanged(float value)
{
	regain_knob.setValue(value);
}

void TimingframeContent::laidbackKnobValueChanged(float value)
{
	settings.latency_laid_back_ms.store(value);
}

void TimingframeContent::laidbackSettingsValueChanged(float value)
{
	laidback_knob.setValue(value);
}

} // GUI::

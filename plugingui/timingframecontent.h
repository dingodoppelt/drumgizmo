/* -*- Mode: c++ -*- */
/***************************************************************************
 *            timingframecontent.h
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
#pragma once

#include "knob.h"
#include "label.h"
#include "labeledcontrol.h"
#include "layout.h"
#include "widget.h"

#include <settings.h>

#include <iomanip>
#include <sstream>

class SettingsNotifier;

namespace GUI
{

class TimingframeContent
	: public Widget
{
public:
	TimingframeContent(Widget* parent,
	                   Settings& settings,
	                   SettingsNotifier& settings_notifier);

private:
	float thightnessKnobToSettings(float value) const;
	float tightnessSettingsToKnob(float value) const;
	float laidbackKnobToSettings(float value) const;
	float laidbackSettingsToKnob(int int_value) const;

	void tightnessKnobValueChanged(float value);
	void tightnessSettingsValueChanged(float value);
	void regainKnobValueChanged(float value);
	void regainSettingsValueChanged(float value);
	void laidbackKnobValueChanged(float value);
	void laidbackSettingsValueChanged(int value);

	GridLayout layout{this, 3, 1};

	LabeledControl tightness{this, "Tightness"};
	LabeledControl regain{this, "Timing Regain"};
	LabeledControl laidback{this, "Laid Back-ness"};

	Knob tightness_knob{&tightness};
	Knob regain_knob{&regain};
	Knob laidback_knob{&laidback};

	Settings& settings;
	SettingsNotifier& settings_notifier;
};

} // GUI::

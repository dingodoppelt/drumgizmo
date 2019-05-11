/* -*- Mode: c++ -*- */
/***************************************************************************
 *            sampleselectionframecontent.h
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
#pragma once

#include "knob.h"
#include "labeledcontrol.h"
#include "layout.h"
#include "widget.h"

struct Settings;
class SettingsNotifier;

namespace GUI
{

class SampleselectionframeContent
	: public Widget
{
public:
	SampleselectionframeContent(Widget* parent,
	                            Settings& settings,
	                            SettingsNotifier& settings_notifier);

private:
	static float constexpr f_distance_factor = 4.f;
	static float constexpr f_recent_factor = 1.f;
	static float constexpr f_random_factor = .5f;

	void fDistanceKnobValueChanged(float value);
	void fRecentKnobValueChanged(float value);
	void fRandomKnobValueChanged(float value);

	void fDistanceSettingsValueChanged(float value);
	void fRecentSettingsValueChanged(float value);
	void fRandomSettingsValueChanged(float value);

	GridLayout layout{this, 3, 1};

	LabeledControl f_distance{this, "fDistance"};
	LabeledControl f_recent{this, "fRecent"};
	LabeledControl f_random{this, "fRandom"};

	Knob f_distance_knob{&f_distance};
	Knob f_recent_knob{&f_recent};
	Knob f_random_knob{&f_random};

	Settings& settings;
	SettingsNotifier& settings_notifier;
};

} // GUI::

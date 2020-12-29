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

#include <dggui/knob.h>
#include <dggui/layout.h>
#include <dggui/widget.h>

#include "labeledcontrol.h"

#include <translation.h>

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
	void fCloseKnobValueChanged(float value);
	void fDiverseKnobValueChanged(float value);
	void fRandomKnobValueChanged(float value);

	void fCloseSettingsValueChanged(float value);
	void fDiverseSettingsValueChanged(float value);
	void fRandomSettingsValueChanged(float value);

	GridLayout layout{this, 3, 1};

	LabeledControl f_close{this, _("pClose")};
	LabeledControl f_diverse{this, _("pDiverse")};
	LabeledControl f_random{this, _("pRandom")};

	Knob f_close_knob{&f_close};
	Knob f_diverse_knob{&f_diverse};
	Knob f_random_knob{&f_random};

	Settings& settings;
	SettingsNotifier& settings_notifier;
};

} // GUI::

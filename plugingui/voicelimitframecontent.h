/* -*- Mode: c++ -*- */
/***************************************************************************
 *            voicelimitframecontent.h
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
#pragma once

#include <translation.h>

#include <dggui/label.h>
#include <dggui/knob.h>
#include <dggui/widget.h>

#include "labeledcontrol.h"

struct Settings;
class SettingsNotifier;

namespace GUI
{

class VoiceLimitFrameContent
	: public Widget
{
public:
	VoiceLimitFrameContent(Widget* parent,
	                       Settings& settings,
	                       SettingsNotifier& settings_notifier);

private:
	void maxvoicesKnobValueChanged(float value);
	void rampdownKnobValueChanged(float value);

	void maxvoicesSettingsValueChanged(float value);
	void rampdownSettingsValueChanged(float value);

	std::size_t convertMaxVoices(float value);

	Settings& settings;
	SettingsNotifier& settings_notifier;

	Label label_text{this};

	GridLayout layout{this, 2, 1};

	LabeledControl lc_max_voices{this, _("Max voices")};
	LabeledControl lc_rampdown_time{this, _("Rampdown time")};

	Knob knob_max_voices{&lc_max_voices};
	Knob knob_rampdown_time{&lc_rampdown_time};
};

} // GUI::

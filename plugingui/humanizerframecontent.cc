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
	attack.setControl(&attack_knob);
	layout.addItem(&attack);

	falloff.resize(80, 80);
	falloff_knob.resize(30, 30);
	falloff_knob.showValue(false);
	falloff.setControl(&falloff_knob);
	layout.addItem(&falloff);

	layout.setPosition(&attack, GridLayout::GridRange{1, 2, 0, 1});
	layout.setPosition(&falloff, GridLayout::GridRange{2, 3, 0, 1});

	CONNECT(this, settings_notifier.velocity_modifier_falloff,
	        &falloff_knob, &Knob::setValue);
	CONNECT(this, settings_notifier.velocity_modifier_weight,
	        &attack_knob, &Knob::setValue);

	CONNECT(&attack_knob, valueChangedNotifier,
	        this, &HumanizerframeContent::attackValueChanged);

	CONNECT(&falloff_knob, valueChangedNotifier,
	        this, &HumanizerframeContent::falloffValueChanged);

}

void HumanizerframeContent::attackValueChanged(float value)
{
	settings.velocity_modifier_weight.store(value);
}

void HumanizerframeContent::falloffValueChanged(float value)
{
	settings.velocity_modifier_falloff.store(value);
}

} // GUI::

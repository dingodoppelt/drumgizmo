/* -*- Mode: c++ -*- */
/***************************************************************************
 *            bleedcontrolframecontent.cc
 *
 *  Wed May 24 14:40:16 CEST 2017
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
#include "bleedcontrolframecontent.h"

#include <settings.h>

#include <limits>

namespace GUI
{

BleedcontrolframeContent::BleedcontrolframeContent(Widget* parent,
                                                   Settings& settings,
                                                   SettingsNotifier& settings_notifier)
	: Widget(parent)
	, slider_width{250}
	, settings(settings)
	, settings_notifier(settings_notifier)
{
	label_text.setText("Amount of Microphone Bleed:");
	label_text.setAlignment(TextAlignment::center);

	button.setText("Apply");
	button.setEnabled(false);

	label_value.setText("0 %");
	label_value.setAlignment(TextAlignment::center);

	// TODO:
	// connect to settings (see diskstreaming frame)
}

void BleedcontrolframeContent::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);

	int slider_button_gap = 10;

	slider_width = 0.8 * width;
	button_width = std::max((int)width - slider_width - slider_button_gap, 0);

	label_text.move(0, 0);
	slider.move(0, 20);
	button.move(slider_width + slider_button_gap, 10);
	label_value.move(0, 40);

	label_text.resize(slider_width, 15);
	slider.resize(slider_width, 15);
	button.resize(button_width, 30);
	label_value.resize(slider_width, 15);

	button.setEnabled(false);
}

void BleedcontrolframeContent::bleedSettingsValueChanged(float value)
{
	slider.setValue(value);

	label_value.setText(std::to_string(100 * value) + " %");
	slider.setColour(Slider::Colour::Blue);

	button.setEnabled(true);
}

void BleedcontrolframeContent::bleedValueChanged(float value)
{
	// TODO: store new value in settings
}

void BleedcontrolframeContent::reloadClicked()
{
	// TODO
}

void BleedcontrolframeContent::reloaded(float)
{
	button.setEnabled(false);
}

} // GUI::

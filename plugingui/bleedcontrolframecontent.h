/* -*- Mode: c++ -*- */
/***************************************************************************
 *            bleedcontrolframecontent.h
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

#include <dggui/label.h>
#include <dggui/slider.h>
#include <dggui/widget.h>

struct Settings;
class SettingsNotifier;

namespace GUI
{

class BleedcontrolframeContent
	: public dggui::Widget
{
public:
	BleedcontrolframeContent(dggui::Widget* parent,
	                         Settings& settings,
	                         SettingsNotifier& settings_notifier);

	// From Widget
	virtual void resize(std::size_t width, std::size_t height) override;

	void setEnabled(bool enabled);

private:
	void bleedSettingsValueChanged(float value);
	void bleedValueChanged(float value);

	bool enabled = true;

	dggui::Label label_text{this};
	dggui::Label label_value{this};

	dggui::Slider slider{this};

	int slider_width;

	Settings& settings;
	SettingsNotifier& settings_notifier;
};

} // GUI::
#pragma once

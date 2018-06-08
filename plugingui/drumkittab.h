/* -*- Mode: c++ -*- */
/***************************************************************************
 *            drumkittab.h
 *
 *  Fri Jun  8 21:50:03 CEST 2018
 *  Copyright 2018 André Nusser
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

#include <memory>
#include <string>

#include "image.h"
#include "label.h"
#include "widget.h"

struct Settings;
class SettingsNotifier;

namespace GUI
{

class Config;

class DrumkitTab
	: public Widget
{
public:
	DrumkitTab(Widget* parent,
	           Settings& settings,
	           SettingsNotifier& settings_notifier,
	           Config& config);

	// From Widget:
	void resize(std::size_t width, std::size_t height) override;
	void buttonEvent(ButtonEvent* buttonEvent) override;
	void scrollEvent(ScrollEvent* scrollEvent) override;

	void loadImageFiles(std::string const& image_file, std::string const& map_file);

private:
	float current_velocity = .5;

	std::unique_ptr<Image> drumkit_image;
	std::unique_ptr<Image> map_image;
	Label velocity_label{this};

	Settings& settings;
	SettingsNotifier& settings_notifier;
	Config& config;

	void triggerAudition(int x, int y);
	void updateVelocityLabel();
};

} // GUI::

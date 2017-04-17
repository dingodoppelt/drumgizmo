/* -*- Mode: c++ -*- */
/***************************************************************************
 *            diskstreamingframecontent.h
 *
 *  Fri Mar 24 21:50:07 CET 2017
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
#pragma once

#include "button.h"
#include "label.h"
#include "slider.h"
#include "widget.h"

struct Settings;
class SettingsNotifier;

namespace GUI
{

class DiskstreamingframeContent : public Widget
{
public:
	DiskstreamingframeContent(Widget* parent,
	                          Settings& settings,
	                          SettingsNotifier& settings_notifier);

	// From Widget
	virtual void resize(std::size_t width, std::size_t height) override;

private:
	void limitSettingsValueChanged(float value);
	void limitValueChanged(float value);
	void reloadClicked();

	Label label_text{this};
	Label label_size{this};

	Slider slider{this};
	Button button{this};

	std::size_t slider_width;
	std::size_t button_width;

	Settings& settings;
	SettingsNotifier& settings_notifier;
};

} // GUI::

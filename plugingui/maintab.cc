/* -*- Mode: c++ -*- */
/***************************************************************************
 *            maintab.cc
 *
 *  Fri Mar 24 20:39:59 CET 2017
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
#include "maintab.h"

namespace GUI
{

MainTab::MainTab(Widget* parent,
                 Settings& settings,
                 SettingsNotifier& settings_notifier,
                 Config& config)
	: Widget(parent)
	, drumkitframe_content{this, settings, settings_notifier, config}
	, statusframe_content{this, settings_notifier}
	, humanizerframe_content{this, settings, settings_notifier}
	, diskstreamingframe_content{this, settings, settings_notifier}
	, settings(settings)
	, settings_notifier(settings_notifier)
{
	layout.setSpacing(0);
	layout.setResizeChildren(true);

	layout.addItem(&drumkit_frame);
	layout.addItem(&status_frame);
	layout.addItem(&humanizer_frame);
	layout.addItem(&diskstreaming_frame);

	auto h1 = 20;
	auto h2 = 22;
	auto h3 = 13;
	auto h4 = 11;
	auto drumkit_range = GridLayout::GridRange{0, 1, 0, h1};
	auto status_range = GridLayout::GridRange{0, 1, h1, h1 + h2};
	auto humanizer_range = GridLayout::GridRange{1, 2, 0, h3};
	auto diskstreaming_range = GridLayout::GridRange{1, 2, h3, h3 + h4};
	layout.setPosition(&drumkit_frame, drumkit_range);
	layout.setPosition(&status_frame, status_range);
	layout.setPosition(&humanizer_frame, humanizer_range);
	layout.setPosition(&diskstreaming_frame, diskstreaming_range);

	drumkit_frame.setTitle("Drumkit");
	status_frame.setTitle("Status");
	humanizer_frame.setTitle("Humanizer");
	diskstreaming_frame.setTitle("Disk streaming");

	drumkit_frame.setContent(&drumkitframe_content);
	status_frame.setContent(&statusframe_content);
	humanizer_frame.setContent(&humanizerframe_content);
	diskstreaming_frame.setContent(&diskstreamingframe_content);

	CONNECT(this, settings_notifier.enable_velocity_modifier,
	        &humanizer_frame, &FrameWidget::setOnSwitch);

	CONNECT(&humanizer_frame, onSwitchChangeNotifier,
	        this, &MainTab::humanizerOnChange);

	CONNECT(this, settings_notifier.disk_cache_enable,
	        &diskstreaming_frame, &FrameWidget::setOnSwitch);

	CONNECT(&diskstreaming_frame, onSwitchChangeNotifier,
	        this, &MainTab::diskStreamingOnChange);
}

void MainTab::humanizerOnChange(bool on)
{
	settings.enable_velocity_modifier.store(on);
}

void MainTab::diskStreamingOnChange(bool on)
{
	settings.disk_cache_enable.store(on);
}

} // GUI::

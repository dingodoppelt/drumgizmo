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
	, bleedcontrolframe_content{this, settings, settings_notifier}
	, resamplingframe_content{this, settings_notifier}
	, timingframe_content{this, settings, settings_notifier}
	, sampleselectionframe_content{this, settings, settings_notifier}
	, visualizerframe_content{this, settings, settings_notifier}
	, settings(settings)
	, settings_notifier(settings_notifier)
{
	layout.setSpacing(0);
	layout.setResizeChildren(true);

	add("Drumkit", drumkit_frame, drumkitframe_content, 15, 0);
	add("Status", status_frame, statusframe_content, 24, 0);
	add("Resampling", resampling_frame, resamplingframe_content, 10, 0);

	add("Velocity Humanizer", humanizer_frame, humanizerframe_content, 10, 1);
	humanizer_frame.setHelpText("Hello World\nThis is a nice World\n... I think");
	add("Timing Humanizer", timing_frame, timingframe_content, 10, 1);
	add("Sample Selection", sampleselection_frame, sampleselectionframe_content, 10, 1);
	add("Visualizer", visualizer_frame, visualizerframe_content, 10, 1);
	add("Bleed Control", bleedcontrol_frame, bleedcontrolframe_content, 9, 1);
	add("Disk Streaming", diskstreaming_frame, diskstreamingframe_content, 10, 1);

	humanizer_frame.setOnSwitch(settings.enable_velocity_modifier);
	bleedcontrol_frame.setOnSwitch(settings.enable_bleed_control);
	resampling_frame.setOnSwitch(settings.enable_resampling);
	timing_frame.setOnSwitch(settings.enable_latency_modifier);

	// FIXME:
	bleedcontrol_frame.setEnabled(false);

	CONNECT(this, settings_notifier.enable_velocity_modifier,
	        &humanizer_frame, &FrameWidget::setOnSwitch);
	CONNECT(this, settings_notifier.enable_resampling,
	        &resampling_frame, &FrameWidget::setOnSwitch);
	CONNECT(this, settings_notifier.has_bleed_control,
	        &bleedcontrol_frame, &FrameWidget::setEnabled);
	CONNECT(&humanizer_frame, onSwitchChangeNotifier,
	        this, &MainTab::humanizerOnChange);
	CONNECT(&bleedcontrol_frame, onSwitchChangeNotifier,
	        this, &MainTab::bleedcontrolOnChange);
	CONNECT(&resampling_frame, onSwitchChangeNotifier,
	        this, &MainTab::resamplingOnChange);
	CONNECT(&timing_frame, onSwitchChangeNotifier,
	        this, &MainTab::timingOnChange);
	CONNECT(&bleedcontrol_frame, onEnabledChanged,
	        &bleedcontrolframe_content, &BleedcontrolframeContent::setEnabled);
}

void MainTab::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);

	// DrumGizmo logo
	Painter painter(*this);
	painter.clear();
	painter.drawImage(width - logo.width(), height - logo.height(), logo);
}

void MainTab::humanizerOnChange(bool on)
{
	settings.enable_velocity_modifier.store(on);
}

void MainTab::bleedcontrolOnChange(bool on)
{
	settings.enable_bleed_control.store(on);
}

void MainTab::resamplingOnChange(bool on)
{
	settings.enable_resampling.store(on);
}

void MainTab::timingOnChange(bool on)
{
	settings.enable_latency_modifier.store(on);
}

void MainTab::add(std::string const& title, FrameWidget& frame, Widget& content,
                  std::size_t height, int column)
{
	layout.addItem(&frame);
	frame.setTitle(title);
	frame.setContent(&content);

	auto grid_start = layout.lastUsedRow(column) + 1;
	auto range = GridLayout::GridRange{column,
	                                   column + 1,
	                                   grid_start,
	                                   grid_start + int(height)};
	layout.setPosition(&frame, range);
}

} // GUI::

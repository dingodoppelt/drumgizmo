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

namespace
{

constexpr char humanizer_tip[] = "\
The first two knobs influence how DrumGizmo simulates the\n\
stamina of a physical drummers, ie. the fact that they\n\
loose power in their strokes when playing fast notes:\n\
  * pAttack: How quickly the velocity gets reduced when\n\
     playing fast notes.\n\
     Lower values result in faster velocity reduction.\n\
  * pRelease: How quickly the drummer regains the velocity\n\
     when there are spaces between the notes.\n\
     Lower values result in faster regain.\n\
\n\
The last knob controls the randomization of the sample selection:\n\
  * pStdDev: The standard-deviation for the sample selection.\n\
     Higher value makes it more likely that a sample further\n\
     away from the input velocity will be played.";

constexpr char timing_tip[] = "\
These three knobs influence how DrumGizmo simulates the tightness\n\
of the drummer. The drifting is defined as the difference between\n\
the perfect metronome (defined by the note positions) and the 'internal'\n\
metronome of the drummer which can then drift back and forth in a\n\
controlled fashion:\n\
  * pTightness: For each note how much is the drummer allowed to drift.\n\
     Higher value make the drummer more tight, ie. drift less.\n\
  * pTimingRegain: Once the drifted, how fast does the drummer's 'internal'\n\
     metronome get back in sync with the perfect metronome.\n\
     Higher values moves the timing back towards perfect faster.\n\
  * pLaidback: Add or subtract a fixed delay in ms to all notes. This will\n\
     alter the feel of a beat.\n\
     Positive values are up-beat, negative values are back on the beat.\n\
\n\
NOTE: Enabling timing humanization will introduce a fixed delay into the\n\
audio stream. So this feature should be disabled when using DrumGizmo in\n\
a real-time scenario such as live with a MIDI drumkit.";

constexpr char sampleselection_tip[] = "\
These three knobs influence how DrumGizmo selects\n\
its samples in the following way:\n\
  * pClose: The importance given to choosing a sample close\n\
     to the actual MIDI value (after humanization)\n\
  * pDiversity: The importance given to choosing samples\n\
     which haven't been played recently.\n\
  * pRandom: The amount of randomness added.";

constexpr char visualizer_tip[] = "\
This graph visualizes the time and velocity offsets of last note played\n\
according to it's ideal input time and velocity.\n\
The green lines indicate the ideal time and velocity positions.\n\
The pink areas indicate the spread of the position and velocity of the\n\
next note in line. The wider the area the more the note can move in time\n\
and velocity.";

constexpr char power_tip[] = "\
This function controls how the input powers are mapped to the powers that\n\
DrumGizmo uses for selecting samples. You can control the function by dragging\n\
and dropping the three colorful control points. Additionally, you can either\n\
use a shelf, which draws a horizontal line from the green and red control\n\
points to the left/right side. Or you can turn off the shelf, and then the\n\
function goes through the lower left corner, then the three control points,\n\
and then the upper right corner, enabling to draw more complicated functions.";

} // end anonymous namespace

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
	, powerframe_content{this, settings, settings_notifier}
	, settings(settings)
	, settings_notifier(settings_notifier)
{
	layout.setSpacing(0);
	layout.setResizeChildren(true);

	add("Drumkit", drumkit_frame, drumkitframe_content, 12, 0);
	add("Status", status_frame, statusframe_content, 14, 0);
	add("Resampling", resampling_frame, resamplingframe_content, 9, 0);
	add("Disk Streaming", diskstreaming_frame, diskstreamingframe_content, 7, 0);
	add("Bleed Control", bleedcontrol_frame, bleedcontrolframe_content, 7, 0);

	add("Velocity Humanizer", humanizer_frame, humanizerframe_content, 8, 1);
	humanizer_frame.setHelpText(humanizer_tip);
	add("Timing Humanizer", timing_frame, timingframe_content, 8, 1);
	timing_frame.setHelpText(timing_tip);
	add("Sample Selection", sampleselection_frame,
	    sampleselectionframe_content, 8, 1);
	sampleselection_frame.setHelpText(sampleselection_tip);
	add("Visualizer", visualizer_frame, visualizerframe_content, 8, 1);
	visualizer_frame.setHelpText(visualizer_tip);
	add("Velocity Curve", power_frame, powerframe_content, 17, 1);
	power_frame.setHelpText(power_tip);

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

	CONNECT(&settings_notifier, enable_powermap,
	        &power_frame, &FrameWidget::setOnSwitch);
	CONNECT(&power_frame, onSwitchChangeNotifier,
	        this, &MainTab::powerOnChange);
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

void MainTab::powerOnChange(bool on)
{
	settings.enable_powermap.store(on);
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

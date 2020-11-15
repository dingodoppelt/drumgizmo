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

#include <translation.h>

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
	, resamplingframe_content{this, settings, settings_notifier}
	, timingframe_content{this, settings, settings_notifier}
	, sampleselectionframe_content{this, settings, settings_notifier}
	, visualizerframe_content{this, settings, settings_notifier}
	, powerframe_content{this, settings, settings_notifier}
	, voicelimit_content{this, settings, settings_notifier}
	, settings(settings)
	, settings_notifier(settings_notifier)
{
	layout.setSpacing(0);

	const std::string humanizer_tip = std::string(
		_("The first two knobs influence how DrumGizmo simulates the\n")) +
		_("stamina of a physical drummers, ie. the fact that they\n") +
		_("loose power in their strokes when playing fast notes:\n") +
		_("  * pAttack: How quickly the velocity gets reduced when\n") +
		_("     playing fast notes.\n") +
		_("     Lower values result in faster velocity reduction.\n") +
		_("  * pRelease: How quickly the drummer regains the velocity\n") +
		_("     when there are spaces between the notes.\n") +
		_("     Lower values result in faster regain.\n") +
		_("\n") +
		_("The last knob controls the randomization of the sample selection:\n") +
		_("  * pStdDev: The standard-deviation for the sample selection.\n") +
		_("     Higher value makes it more likely that a sample further\n") +
		_("     away from the input velocity will be played.");

	const std::string timing_tip = std::string(
		_("These three knobs influence how DrumGizmo simulates the tightness\n")) +
		_("of the drummer.\n") +
		_("The drifting is defined as the difference between\n") +
		_("the perfect metronome (defined by the note positions) and the 'internal'\n") +
		_("metronome of the drummer which can then drift back and forth in a\n") +
		_("controlled fashion:\n") +
		_("  * pTightness: For each note how much is the drummer allowed to drift.\n") +
		_("     Higher value make the drummer more tight, ie. drift less.\n") +
		_("  * pTimingRegain: Once the drifted, how fast does the drummer's 'internal'\n") +
		_("     metronome get back in sync with the perfect metronome.\n") +
		_("     Higher values moves the timing back towards perfect faster.\n") +
		_("  * pLaidback: Add or subtract a fixed delay in ms to all notes. This will\n") +
		_("     alter the feel of a beat.\n") +
		_("     Positive values are up-beat, negative values are back on the beat.\n") +
		_("\n") +
		_("NOTE: Enabling timing humanization will introduce a fixed delay into the\n") +
		_("audio stream.\n") +
		_("So this feature should be disabled when using DrumGizmo in\n") +
		_("a real-time scenario such as live with a MIDI drumkit.");

	const std::string sampleselection_tip = std::string(
		_("These three knobs influence how DrumGizmo selects\n")) +
		_("its samples in the following way:\n") +
		_("  * pClose: The importance given to choosing a sample close\n") +
		_("     to the actual MIDI value (after humanization)\n") +
		_("  * pDiversity: The importance given to choosing samples\n") +
		_("     which haven't been played recently.\n") +
		_("  * pRandom: The amount of randomness added.");

	const std::string visualizer_tip = std::string(
		_("This graph visualizes the time and velocity offsets of last note\n")) +
		_("played according to it's ideal input time and velocity.\n") +
		_("The green lines indicate the ideal time and velocity positions.\n") +
		_("The pink areas indicate the spread of the position and velocity\n") +
		_("of the next note in line.\n") +
		_("The wider the area the more the note can move in time and velocity.");

	const std::string power_tip = std::string(
		_("This function controls how the input powers are mapped to the powers\n")) +
		_("that DrumGizmo uses for selecting samples.\n") +
		_("You can control the function by dragging and dropping the\n") +
		_("three colorful control points.\n") +
		_("Additionally, you can either use a shelf, which draws a horizontal line\n") +
		_("from the green and red control points to the left/right side.\n") +
		_("Or you can turn off the shelf, and then the function goes through\n") +
		_("the lower left corner, then the three control points, and then\n") +
		_("the upper right corner, enabling to draw more complicated functions.");

	const std::string voice_limit_tip = std::string(
		_("This feature controls how many voices can simultaneously be in play for a given\n")) +
		_("instrument. When this feature is active, Drumgizmo will silence any excess \n") +
		_("voices to ease the burden of processing.\n") +
		_("\n") +
		_("This feature works on a per-instrument basis, e.g., voices played on the bass\n") +
		_("drum can only be silenced by other bass drum hits, and not by the snare.\n") +
		_("\n") +
		_("  * Max voices: The maximum number of voices that should be allowed to play.\n") +
		_("  * Rampdown time: How many seconds it takes to silence a voice.");

	layout.setResizeChildren(true);

	//Left column...
	add(_("Drumkit"), drumkit_frame, drumkitframe_content, 14, 0);
	add(_("Status"), status_frame, statusframe_content, 12, 0);
	add(_("Resampling"), resampling_frame, resamplingframe_content, 10, 0);
	add(_("Voice Limit"), voicelimit_frame, voicelimit_content, 10, 0);
	voicelimit_frame.setHelpText(voice_limit_tip);
	add(_("Disk Streaming"), diskstreaming_frame, diskstreamingframe_content, 9, 0);
	add(_("Bleed Control"), bleedcontrol_frame, bleedcontrolframe_content, 9, 0);

	//Right column
	add(_("Velocity Humanizer"), humanizer_frame, humanizerframe_content,10, 1);
	humanizer_frame.setHelpText(humanizer_tip);

	add(_("Timing Humanizer"), timing_frame, timingframe_content, 10, 1);
	timing_frame.setHelpText(timing_tip);

	add(_("Sample Selection"), sampleselection_frame,
	    sampleselectionframe_content, 10, 1);
	sampleselection_frame.setHelpText(sampleselection_tip);

	add(_("Visualizer"), visualizer_frame, visualizerframe_content, 14, 1);
	visualizer_frame.setHelpText(visualizer_tip);

	add(_("Velocity Curve"), power_frame, powerframe_content, 20, 1);
	power_frame.setHelpText(power_tip);

	humanizer_frame.setOnSwitch(settings.enable_velocity_modifier);
	bleedcontrol_frame.setOnSwitch(settings.enable_bleed_control);
	resampling_frame.setOnSwitch(settings.enable_resampling);
	timing_frame.setOnSwitch(settings.enable_latency_modifier);
	voicelimit_frame.setOnSwitch(settings.enable_voice_limit);

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
	CONNECT(&voicelimit_frame, onSwitchChangeNotifier,
	        this, &MainTab::voicelimitOnChange);

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

void MainTab::voicelimitOnChange(bool status)
{
	settings.enable_voice_limit.store(status);
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

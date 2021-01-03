/* -*- Mode: c++ -*- */
/***************************************************************************
 *            maintab.h
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
#pragma once

#include <dggui/widget.h>
#include <dggui/layout.h>
#include <dggui/frame.h>

#include "drumkitframecontent.h"
#include "statusframecontent.h"
#include "humanizerframecontent.h"
#include "diskstreamingframecontent.h"
#include "bleedcontrolframecontent.h"
#include "resamplingframecontent.h"
#include "timingframecontent.h"
#include "sampleselectionframecontent.h"
#include "visualizerframecontent.h"
#include "powerwidget.h"
#include "voicelimitframecontent.h"

struct Settings;
class SettingsNotifier;
class Config;

namespace GUI
{

class MainTab
	: public dggui::Widget
{
public:
	MainTab(dggui::Widget* parent,
	        Settings& settings,
	        SettingsNotifier& settings_notifier,
	        Config& config);

	// From Widget:
	void resize(std::size_t width, std::size_t height) override;

private:
	void humanizerOnChange(bool on);
	void bleedcontrolOnChange(bool on);
	void resamplingOnChange(bool on);
	void timingOnChange(bool on);
	void powerOnChange(bool on);
	void voicelimitOnChange(bool status);

	dggui::Image logo{":resources/logo.png"};

	dggui::GridLayout layout{this, 2, 64};

	dggui::FrameWidget drumkit_frame{this, false};
	dggui::FrameWidget status_frame{this, false};
	dggui::FrameWidget diskstreaming_frame{this, false};
	dggui::FrameWidget bleedcontrol_frame{this, true};
	dggui::FrameWidget resampling_frame{this, true};
	dggui::FrameWidget humanizer_frame{this, true, true};
	dggui::FrameWidget timing_frame{this, true, true};
	dggui::FrameWidget sampleselection_frame{this, false, true};
	dggui::FrameWidget visualizer_frame{this, false, true};
	dggui::FrameWidget power_frame{this, true, true};
	dggui::FrameWidget voicelimit_frame{this, true, true};

	DrumkitframeContent drumkitframe_content;
	StatusframeContent statusframe_content;
	HumanizerframeContent humanizerframe_content;
	DiskstreamingframeContent diskstreamingframe_content;
	BleedcontrolframeContent bleedcontrolframe_content;
	ResamplingframeContent resamplingframe_content;
	TimingframeContent timingframe_content;
	SampleselectionframeContent sampleselectionframe_content;
	VisualizerframeContent visualizerframe_content;
	PowerWidget powerframe_content;
	VoiceLimitFrameContent voicelimit_content;

	void add(std::string const& title, dggui::FrameWidget& frame,
	         dggui::Widget& content, std::size_t height, int column);

	Settings& settings;
	SettingsNotifier& settings_notifier;
};

} // GUI::

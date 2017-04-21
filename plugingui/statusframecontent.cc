/* -*- Mode: c++ -*- */
/***************************************************************************
 *            statusframecontent.cc
 *
 *  Fri Mar 24 21:49:50 CET 2017
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
#include "statusframecontent.h"

namespace GUI
{

StatusframeContent::StatusframeContent(
    Widget* parent, SettingsNotifier& settings_notifier)
    : Widget(parent), settings_notifier(settings_notifier)
{
	CONNECT(this, settings_notifier.drumkit_load_status,
	        this, &StatusframeContent::updateDrumkitLoadStatus);
	CONNECT(this, settings_notifier.midimap_load_status,
	        this, &StatusframeContent::updateMidimapLoadStatus);
	CONNECT(this, settings_notifier.samplerate,
	        this, &StatusframeContent::updateSamplerate);
	CONNECT(this, settings_notifier.enable_resampling,
	        this, &StatusframeContent::updateResamplingEnabled);
	CONNECT(this, settings_notifier.number_of_underruns,
	        this, &StatusframeContent::updateNumberOfUnderruns);

	text_field.move(0, 0);
	text_field.setReadOnly(true);

	updateContent();
	text_field.show();
}

void StatusframeContent::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);
	text_field.resize(width, height);
}

void StatusframeContent::updateContent()
{
	text_field.setText(
		"Drumkit load status: " + drumkit_load_status + "\n"
		"Midimap load status: " + midimap_load_status + "\n"
		"Samplerate: " + samplerate + "\n"
		"Resampling enabled: " + resampling_enabled + "\n"
		"Number of underruns: " + number_of_underruns + "\n"
	);
}

void StatusframeContent::updateDrumkitLoadStatus(LoadStatus load_status)
{
	switch(load_status)
	{
	case LoadStatus::Idle:
		drumkit_load_status = "idle";
		break;
	case LoadStatus::Loading:
		drumkit_load_status = "loading";
		break;
	case LoadStatus::Done:
		drumkit_load_status = "done";
		break;
	case LoadStatus::Error:
		drumkit_load_status = "error";
		break;
	}

	updateContent();
}

void StatusframeContent::updateMidimapLoadStatus(LoadStatus load_status)
{
	switch(load_status)
	{
	case LoadStatus::Idle:
		midimap_load_status = "idle";
		break;
	case LoadStatus::Loading:
		midimap_load_status = "loading";
		break;
	case LoadStatus::Done:
		midimap_load_status = "done";
		break;
	case LoadStatus::Error:
		midimap_load_status = "error";
		break;
	}

	updateContent();
}

void StatusframeContent::updateSamplerate(double samplerate)
{
	this->samplerate = std::to_string((std::size_t)samplerate);

	updateContent();
}

void StatusframeContent::updateResamplingEnabled(bool enable_resampling)
{
	this->resampling_enabled = enable_resampling ? "yes" : "no";

	updateContent();
}

void StatusframeContent::updateNumberOfUnderruns(
    std::size_t number_of_underruns)
{
	this->number_of_underruns = std::to_string(number_of_underruns);

	updateContent();
}

} // GUI::

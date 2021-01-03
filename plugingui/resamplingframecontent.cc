/* -*- Mode: c++ -*- */
/***************************************************************************
 *            resamplingframecontent.cc
 *
 *  Fri May  5 23:43:28 CEST 2017
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
#include "resamplingframecontent.h"

#include <settings.h>

#include <translation.h>

namespace GUI
{

ResamplingframeContent::ResamplingframeContent(dggui::Widget* parent,
                                               Settings& settings,
                                               SettingsNotifier& settings_notifier)
	: dggui::Widget(parent)
	, settings(settings)
	, settings_notifier(settings_notifier)
{
	CONNECT(this, settings_notifier.drumkit_samplerate,
	        this, &ResamplingframeContent::updateDrumkitSamplerate);
	CONNECT(this, settings_notifier.samplerate,
	        this, &ResamplingframeContent::updateSessionSamplerate);
	CONNECT(this, settings_notifier.resampling_recommended,
	        this, &ResamplingframeContent::updateResamplingRecommended);
	CONNECT(this, settings_notifier.resampling_quality,
	        this, &ResamplingframeContent::updateResamplingQuality);

	text_field.move(0, 0);
	text_field.setReadOnly(true);

	quality_knob.resize(30, 30);
	quality_knob.setDefaultValue(0.7f);
	quality_knob.showValue(false);

	quality_label.setText(_("Quality"));
	quality_label.setAlignment(dggui::TextAlignment::center);
	quality_label.resize(40, 16);

	CONNECT(&quality_knob, valueChangedNotifier,
	        this, &ResamplingframeContent::valueChangedNotifier);

	updateContent();
	text_field.show();
}

void ResamplingframeContent::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);
	text_field.resize(width - 50, height);
	quality_knob.move(width - 36, 20);
	quality_label.move(width - 40, 0);
}

void ResamplingframeContent::updateContent()
{
	text_field.setText(
		 _("Session samplerate:   ") + session_samplerate + "\n" +
		 _("Drumkit samplerate:   ") + drumkit_samplerate + "\n" +
		 _("Resampling recommended:   ") + resampling_recommended + "\n"
	);
}

void ResamplingframeContent::updateDrumkitSamplerate(std::size_t drumkit_samplerate)
{
	this->drumkit_samplerate = drumkit_samplerate == 0
		? ""
		: std::to_string(drumkit_samplerate);

	updateContent();
}

void ResamplingframeContent::updateSessionSamplerate(double samplerate)
{
	this->session_samplerate = std::to_string((std::size_t)samplerate);

	updateContent();
}

void ResamplingframeContent::updateResamplingRecommended(bool resampling_recommended)
{
	this->resampling_recommended = resampling_recommended ? _("Yes") : _("No");

	updateContent();
}

void ResamplingframeContent::updateResamplingQuality(float resampling_quality)
{
	quality_knob.setValue(resampling_quality);
}

void ResamplingframeContent::valueChangedNotifier(float value)
{
	settings.resampling_quality.store(value);
}

} // GUI::

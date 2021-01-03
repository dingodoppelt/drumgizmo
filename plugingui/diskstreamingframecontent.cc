/* -*- Mode: c++ -*- */
/***************************************************************************
 *            diskstreamingframecontent.cc
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
#include "diskstreamingframecontent.h"

#include <settings.h>

#include <limits>

#include <translation.h>

namespace GUI
{

DiskstreamingframeContent::DiskstreamingframeContent(dggui::Widget* parent,
                                                     Settings& settings,
                                                     SettingsNotifier& settings_notifier)
	: dggui::Widget(parent)
	, slider_width{250}
	, settings(settings)
	, settings_notifier(settings_notifier)
{
	label_text.setText(_("Cache limit (max memory usage):"));
	label_text.setAlignment(dggui::TextAlignment::center);

	button.setText(_("Apply"));
	button.setEnabled(false);

	label_value.setText(_("0 MB"));
	label_value.setAlignment(dggui::TextAlignment::center);

	CONNECT(this, settings_notifier.disk_cache_upper_limit,
	        this, &DiskstreamingframeContent::limitSettingsValueChanged);

	CONNECT(&slider, valueChangedNotifier,
	        this, &DiskstreamingframeContent::limitValueChanged);

	CONNECT(&button, clickNotifier,
	        this, &DiskstreamingframeContent::reloadClicked);

	CONNECT(this, settings_notifier.reload_counter,
	        this, &DiskstreamingframeContent::reloaded);

	// TODO:
	//	CONNECT(this, settings_notifier.disk_cache_chunk_size,
	//	        this, &DGWindow::chunkSettingsValueChanged);
	//	CONNECT(this, settings_notifier.number_of_underruns,
	//	        this, &DGWindow::underrunsChanged);
}

void DiskstreamingframeContent::resize(std::size_t width, std::size_t height)
{
	dggui::Widget::resize(width, height);

	int slider_button_gap = 10;

	slider_width = 0.8 * width;
	button_width = std::max((int)width - slider_width - slider_button_gap, 0);

	label_text.move(0, 0);
	slider.move(0, 20);
	button.move(slider_width + slider_button_gap, 10);
	label_value.move(0, 40);

	label_text.resize(slider_width, 15);
	slider.resize(slider_width, 15);
	button.resize(button_width, 30);
	label_value.resize(slider_width, 15);

	button.setEnabled(false);
}

void DiskstreamingframeContent::limitSettingsValueChanged(std::size_t value)
{
	float new_slider_value = (float)(value - min_limit)/(max_limit - min_limit);
	slider.setValue(new_slider_value);

	if(new_slider_value < 0.99)
	{
		int value_in_mb = value/(1024 * 1024);
		label_value.setText(std::to_string(value_in_mb) + " MB");
		slider.setColour(dggui::Slider::Colour::Blue);
	}
	else
	{
		label_value.setText(_("Unlimited"));
		slider.setColour(dggui::Slider::Colour::Grey);
	}

	button.setEnabled(true);
}

void DiskstreamingframeContent::limitValueChanged(float value)
{
	std::size_t new_limit = value < 0.99 ?
		value * (max_limit - min_limit) + min_limit :
		std::numeric_limits<std::size_t>::max();

	settings.disk_cache_upper_limit.store(new_limit);
}

void DiskstreamingframeContent::reloadClicked()
{
	settings.reload_counter++;
}

void DiskstreamingframeContent::reloaded(std::size_t)
{
	button.setEnabled(false);
}

} // GUI::

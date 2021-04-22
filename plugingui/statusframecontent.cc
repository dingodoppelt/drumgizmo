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

#include <translation.h>

namespace GUI
{

StatusframeContent::StatusframeContent(dggui::Widget* parent,
                                       SettingsNotifier& settings_notifier)
	: dggui::Widget(parent)
	, settings_notifier(settings_notifier)
{
	CONNECT(this, settings_notifier.drumkit_load_status,
	        this, &StatusframeContent::updateDrumkitLoadStatus);
	CONNECT(this, settings_notifier.drumkit_name,
	        this, &StatusframeContent::updateDrumkitName);
	CONNECT(this, settings_notifier.drumkit_description,
	        this, &StatusframeContent::updateDrumkitDescription);
	CONNECT(this, settings_notifier.drumkit_version,
	        this, &StatusframeContent::updateDrumkitVersion);
	CONNECT(this, settings_notifier.midimap_load_status,
	        this, &StatusframeContent::updateMidimapLoadStatus);
	CONNECT(this, settings_notifier.buffer_size,
	        this, &StatusframeContent::updateBufferSize);
	CONNECT(this, settings_notifier.number_of_underruns,
	        this, &StatusframeContent::updateNumberOfUnderruns);

	CONNECT(this, settings_notifier.load_status_text,
	        this, &StatusframeContent::loadStatusTextChanged);

	text_field.move(0, 0);
	text_field.setReadOnly(true);

	updateContent();
	text_field.show();
}

void StatusframeContent::resize(std::size_t width, std::size_t height)
{
	dggui::Widget::resize(width, height);
	text_field.resize(width, height);
}

void StatusframeContent::updateContent()
{
	text_field.setText(
		 _("Drumkit status:   ") + drumkit_load_status + "\n" +
		//  _("Midimap status: ") + midimap_load_status + "\n" +
		 _("Drumkit name:   ") + drumkit_name + "\n" +
		 _("Drumkit description:   ") + drumkit_description + "\n" +
		//  _("Drumkit version:   ") + drumkit_version + "\n" +
		 _("Session buffer size:   ") + buffer_size + "\n" +
		 _("Number of underruns: ") + number_of_underruns + "\n" +
		 _("Messages:\n") + messages
	);
}

void StatusframeContent::updateDrumkitLoadStatus(LoadStatus load_status)
{
	switch(load_status)
	{
	case LoadStatus::Idle:
		drumkit_load_status = _("No Kit Loaded");
		break;
	case LoadStatus::Parsing:
	case LoadStatus::Loading:
		drumkit_load_status = _("Loading...");
		break;
	case LoadStatus::Done:
		drumkit_load_status = _("Ready");
		break;
	case LoadStatus::Error:
		drumkit_load_status = _("Error");
		break;
	}

	updateContent();
}

void StatusframeContent::updateDrumkitName(const std::string& drumkit_name)
{
	this->drumkit_name = drumkit_name;

	updateContent();
}

void StatusframeContent::updateDrumkitDescription(const std::string& drumkit_description)
{
	this->drumkit_description = drumkit_description;

	updateContent();
}

void StatusframeContent::updateDrumkitVersion(const std::string& drumkit_version)
{
	this->drumkit_version = drumkit_version;

	updateContent();
}

void StatusframeContent::updateMidimapLoadStatus(LoadStatus load_status)
{
	switch(load_status)
	{
	case LoadStatus::Idle:
		midimap_load_status = _("No Midimap Loaded");
		break;
	case LoadStatus::Parsing:
	case LoadStatus::Loading:
		midimap_load_status = _("Loading...");
		break;
	case LoadStatus::Done:
		midimap_load_status = _("Ready");
		break;
	case LoadStatus::Error:
		midimap_load_status = _("Error");
		break;
	}

	updateContent();
}

void StatusframeContent::updateBufferSize(std::size_t buffer_size)
{
	this->buffer_size = std::to_string(buffer_size);

	updateContent();
}

void StatusframeContent::updateNumberOfUnderruns(std::size_t number_of_underruns)
{
	this->number_of_underruns = std::to_string(number_of_underruns);

	updateContent();
}

void StatusframeContent::loadStatusTextChanged(const std::string& text)
{
	messages = text;
	updateContent();
}

} // GUI::

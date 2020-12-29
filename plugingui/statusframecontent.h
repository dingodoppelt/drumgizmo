/* -*- Mode: c++ -*- */
/***************************************************************************
 *            statusframecontent.h
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
#pragma once

#include "settings.h"

#include <dggui/textedit.h>
#include <dggui/widget.h>

class SettingsNotifier;

namespace GUI
{

class StatusframeContent : public Widget
{
public:
	StatusframeContent(Widget* parent, SettingsNotifier& settings_notifier);

	// From Widget
	virtual void resize(std::size_t width, std::size_t height) override;

	void updateContent();

	void updateDrumkitLoadStatus(LoadStatus load_status);
	void updateDrumkitName(const std::string& drumkit_name);
	void updateDrumkitDescription(const std::string& drumkit_description);
	void updateDrumkitVersion(const std::string& drumkit_version);
	void updateMidimapLoadStatus(LoadStatus load_status);
	void updateBufferSize(std::size_t buffer_size);
	void updateNumberOfUnderruns(std::size_t number_of_underruns);
	void loadStatusTextChanged(const std::string& text);

private:
	TextEdit text_field{this};

	SettingsNotifier& settings_notifier;

	std::string drumkit_load_status;
	std::string drumkit_name;
	std::string drumkit_description;
	std::string drumkit_version;
	std::string midimap_load_status;
	std::string buffer_size;
	std::string number_of_underruns;
	std::string messages;
};

} // GUI::

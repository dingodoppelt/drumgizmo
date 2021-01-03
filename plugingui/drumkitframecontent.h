/* -*- Mode: c++ -*- */
/***************************************************************************
 *            drumkitframecontent.h
 *
 *  Fri Mar 24 21:49:42 CET 2017
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

#include <settings.h>

#include <dggui/button.h>
#include <dggui/label.h>
#include <dggui/lineedit.h>
#include <dggui/progressbar.h>
#include <dggui/widget.h>

#include "filebrowser.h"

namespace GUI
{

class Config;

class BrowseFile
	: public dggui::Widget
{
public:
	BrowseFile(dggui::Widget* parent);

	// From Widget
	virtual void resize(std::size_t width, std::size_t height) override;

	std::size_t getLineEditWidth();
	std::size_t getButtonWidth();

	dggui::Button& getBrowseButton();
	dggui::LineEdit& getLineEdit();

private:
	dggui::HBoxLayout layout{this};

	dggui::LineEdit lineedit{this};
	dggui::Button browse_button{this};

	int lineedit_width;
	int button_width;
	int gap{10};
};

class DrumkitframeContent
	: public dggui::Widget
{
public:
	DrumkitframeContent(dggui::Widget* parent,
	                    Settings& settings,
	                    SettingsNotifier& settings_notifier,
	                    Config& config);

	// From Widget
	virtual void resize(std::size_t width, std::size_t height) override;

	void kitBrowseClick();
	void midimapBrowseClick();

private:
	void defaultPathChanged(const std::string& path);

	void selectKitFile(const std::string& filename);
	void selectMapFile(const std::string& filename);

	void setDrumKitLoadStatus(LoadStatus load_status);
	void setMidiMapLoadStatus(LoadStatus load_status);

	dggui::VBoxLayout layout{this};

	dggui::Label drumkit_caption{this};
	dggui::Label midimap_caption{this};
	BrowseFile drumkit_file{this};
	BrowseFile midimap_file{this};
	dggui::ProgressBar drumkit_file_progress{this};
	dggui::ProgressBar midimap_file_progress{this};

	FileBrowser file_browser{this};

	Settings& settings;
	SettingsNotifier& settings_notifier;
	Config& config;
};

} // GUI::

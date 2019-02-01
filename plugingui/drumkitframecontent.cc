/* -*- Mode: c++ -*- */
/***************************************************************************
 *            drumkitframecontent.cc
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
#include "drumkitframecontent.h"

#include <settings.h>

#include "label.h"
#include "pluginconfig.h"

namespace GUI
{

BrowseFile::BrowseFile(Widget* parent)
	: Widget(parent)
{
	layout.setResizeChildren(false);
	layout.setVAlignment(VAlignment::center);
	layout.setSpacing(gap);

	layout.addItem(&lineedit);
	layout.addItem(&browse_button);

	browse_button.setText("Browse...");
}

void BrowseFile::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);

	lineedit_width = std::max((int)(0.77 * (int)width - gap), 0);
	button_width = std::max((int)width - lineedit_width - gap, 0);

	lineedit.resize(lineedit_width, 29);
	browse_button.resize(button_width, 30);

	layout.layout();
}

std::size_t BrowseFile::getLineEditWidth()
{
	return lineedit_width;
}

std::size_t BrowseFile::getButtonWidth()
{
	return button_width;
}

Button& BrowseFile::getBrowseButton()
{
	return browse_button;
}

LineEdit& BrowseFile::getLineEdit()
{
	return lineedit;
}

DrumkitframeContent::DrumkitframeContent(Widget* parent,
                                         Settings& settings,
                                         SettingsNotifier& settings_notifier,
                                         Config& config)
	: Widget(parent)
	, settings(settings)
	, settings_notifier(settings_notifier)
	, config(config)
{
	layout.setHAlignment(HAlignment::left);

	drumkit_caption.setText("Drumkit file:");
	midimap_caption.setText("Midimap file:");

	layout.addItem(&drumkit_caption);
	layout.addItem(&drumkit_file);
	layout.addItem(&drumkit_file_progress);
	layout.addItem(&midimap_caption);
	layout.addItem(&midimap_file);
	layout.addItem(&midimap_file_progress);

	CONNECT(&drumkit_file.getBrowseButton(), clickNotifier,
	        this, &DrumkitframeContent::kitBrowseClick);

	CONNECT(&midimap_file.getBrowseButton(), clickNotifier,
	        this, &DrumkitframeContent::midimapBrowseClick);


	CONNECT(this, settings_notifier.drumkit_file,
	        &drumkit_file.getLineEdit(), &LineEdit::setText);
	CONNECT(this, settings_notifier.drumkit_load_status,
	        this, &DrumkitframeContent::setDrumKitLoadStatus);

	CONNECT(this, settings_notifier.midimap_file,
	        &midimap_file.getLineEdit(), &LineEdit::setText);
	CONNECT(this, settings_notifier.midimap_load_status,
	        this, &DrumkitframeContent::setMidiMapLoadStatus);

	CONNECT(this, settings_notifier.number_of_files,
	        &drumkit_file_progress, &ProgressBar::setTotal);

	CONNECT(this, settings_notifier.number_of_files_loaded,
	        &drumkit_file_progress, &ProgressBar::setValue);

	CONNECT(this, file_browser. defaultPathChangedNotifier,
	        this, &DrumkitframeContent::defaultPathChanged);

	midimap_file_progress.setTotal(2);

	file_browser.resize(450, 350);
	file_browser.setFixedSize(450, 350);
}

void DrumkitframeContent::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);

	drumkit_caption.resize(width, 15);
	drumkit_file.resize(width, 37);
	drumkit_file_progress.resize(drumkit_file.getLineEditWidth(), 11);

	midimap_caption.resize(width, 15);
	midimap_file.resize(width, 37);
	midimap_file_progress.resize(drumkit_file.getLineEditWidth(), 11);

	layout.layout();
}

void DrumkitframeContent::kitBrowseClick()
{
	std::string path = drumkit_file.getLineEdit().getText();

	if(path == "")
	{
		path = midimap_file.getLineEdit().getText();
	}

	if(path == "")
	{
		path = config.defaultKitPath;
	}

	file_browser.setPath(path);
	file_browser.fileSelectNotifier.disconnect(this);

	CONNECT(&file_browser, fileSelectNotifier,
	        this, &DrumkitframeContent::selectKitFile);
	file_browser.show();
}

void DrumkitframeContent::midimapBrowseClick()
{
	std::string path = midimap_file.getLineEdit().getText();

	if(path == "")
	{
		path = drumkit_file.getLineEdit().getText();
	}

	if(path == "")
	{
		path = config.defaultKitPath;
	}

	file_browser.setPath(path);
	file_browser.fileSelectNotifier.disconnect(this);
	CONNECT(&file_browser, fileSelectNotifier,
	        this, &DrumkitframeContent::selectMapFile);
	file_browser.show();
}

void DrumkitframeContent::defaultPathChanged(const std::string& path)
{
	config.defaultKitPath = path;
	config.save();
}

void DrumkitframeContent::selectKitFile(const std::string& filename)
{
	config.save();

	settings.drumkit_file.store(filename);
	settings.reload_counter++;
}

void DrumkitframeContent::selectMapFile(const std::string& filename)
{
	config.save();

	settings.midimap_file.store(filename);
}

void DrumkitframeContent::setDrumKitLoadStatus(LoadStatus load_status)
{
	ProgressBarState state = ProgressBarState::Blue;
	switch(load_status)
	{
	case LoadStatus::Idle:
	case LoadStatus::Loading:
		state = ProgressBarState::Blue;
		break;
	case LoadStatus::Done:
		state = ProgressBarState::Green;
		break;
	case LoadStatus::Error:
		state = ProgressBarState::Red;
		break;
	}

	drumkit_file_progress.setState(state);
}

void DrumkitframeContent::setMidiMapLoadStatus(LoadStatus load_status)
{
	ProgressBarState state = ProgressBarState::Blue;
	switch(load_status)
	{
	case LoadStatus::Idle:
		midimap_file_progress.setValue(0);
		break;
	case LoadStatus::Loading:
		midimap_file_progress.setValue(1);
		state = ProgressBarState::Blue;
		break;
	case LoadStatus::Done:
		midimap_file_progress.setValue(2);
		state = ProgressBarState::Green;
		break;
	case LoadStatus::Error:
		midimap_file_progress.setValue(2);
		state = ProgressBarState::Red;
		break;
	}

	midimap_file_progress.setState(state);
}

} // GUI::

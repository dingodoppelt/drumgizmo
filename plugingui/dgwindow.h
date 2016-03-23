/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            dgwindow.h
 *
 *  Mon Nov 23 20:30:45 CET 2015
 *  Copyright 2015 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#pragma once

#include "window.h"

#include "label.h"
#include "lineedit.h"
#include "checkbox.h"
#include "button.h"
#include "knob.h"
#include "progressbar.h"
#include "filebrowser.h"
#include "layout.h"

#include <settings.h>

class MessageHandler;

namespace GUI {

class Config;
class Header;
class File;

class DGWindow : public Window {
public:
	DGWindow(void* native_window, MessageHandler& messageHandler, Config& config,
	         Settings& settings);

	Header* header;

	File* drumkitFile;
	LineEdit* lineedit;
	ProgressBar* drumkitFileProgress;

	File* midimapFile;
	LineEdit* lineedit2;
	ProgressBar* midimapFileProgress;

	// Humanized velocity controls:
	CheckBox* velocityCheck;
	Knob* attackKnob;
	Knob* falloffKnob;
	FileBrowser* fileBrowser;

protected:
	// From Widget:
	void repaintEvent(RepaintEvent* repaintEvent) override;

private:
	void attackValueChanged(float value);
	void falloffValueChanged(float value);
	void velocityCheckClick(bool checked);
	void kitBrowseClick();
	void midimapBrowseClick();
	void selectKitFile(const std::string& filename);
	void selectMapFile(const std::string& filename);

	MessageHandler& messageHandler;
	Config& config;

	VBoxLayout layout{this};

	Image back{":bg.png"};
	Image logo{":logo.png"};

	Settings& settings;
};

} // GUI::

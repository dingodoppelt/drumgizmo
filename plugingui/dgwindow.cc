/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            dgwindow.cc
 *
 *  Mon Nov 23 20:30:45 CET 2015
 *  Copyright 2015 Bent Bisballe Nyeng
 *  deva@aasimon.org
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
#include "dgwindow.h"

#include "knob.h"
#include "verticalline.h"
#include "../version.h"

#include "pluginconfig.h"
#include <memchecker.h>
#include <cpp11fix.h>

namespace GUI {

class LabeledControl
	: public Widget
{
public:
	LabeledControl(Widget* parent, const std::string& name)
		: Widget(parent)
	{
		layout.setResizeChildren(false);
		layout.setHAlignment(HAlignment::center);

		caption.setText(name);
		caption.resize(100, 20);
		caption.setAlignment(TextAlignment::center);
		layout.addItem(&caption);
	}

	void setControl(Widget* control)
	{
		layout.addItem(control);
	}

	VBoxLayout layout{this};

	Label caption{this};
};

class File
	: public Widget
{
public:
	File(Widget* parent)
		: Widget(parent)
	{
		layout.setResizeChildren(false);
		layout.setVAlignment(VAlignment::center);

		lineedit.resize(243, 29);
		layout.addItem(&lineedit);

		browseButton.setText("Browse...");
		browseButton.resize(85, 41);
		layout.addItem(&browseButton);
	}

	HBoxLayout layout{this};

	LineEdit lineedit{this};
	Button browseButton{this};
};

class HumanizeControls
	: public Widget
{
public:
	HumanizeControls(Widget* parent)
		: Widget(parent)
	{
		layout.setResizeChildren(false);
		layout.setVAlignment(VAlignment::center);

		velocity.resize(80, 80);
		velocityCheck.resize(59, 38);
		velocity.setControl(&velocityCheck);
		layout.addItem(&velocity);

		attack.resize(80, 80);
		attackKnob.resize(60, 60);
		attack.setControl(&attackKnob);
		layout.addItem(&attack);

		falloff.resize(80, 80);
		falloffKnob.resize(60, 60);
		falloff.setControl(&falloffKnob);
		layout.addItem(&falloff);
	}

	HBoxLayout layout{this};

	LabeledControl velocity{this, "Humanizer"};
	LabeledControl attack{this, "Attack"};
	LabeledControl falloff{this, "Release"};

	CheckBox velocityCheck{&velocity};
	Knob attackKnob{&attack};
	Knob falloffKnob{&falloff};
};

class DiskStreamingControls
	: public Widget
	, private MemChecker
{
public:
	DiskStreamingControls(Widget* parent)
		: Widget(parent)
	{
		layout.setResizeChildren(false);
		layout.setVAlignment(VAlignment::center);

		streamer.resize(80, 80);
		streamerCheck.resize(59, 38);
		streamer.setControl(&streamerCheck);
		layout.addItem(&streamer);

		limit.resize(80, 80);
		float free_mem = calcFreeMemory() / (1024.0 * 1024.0);
		limitKnob.setRange(std::min(500.0, free_mem / 2.0), free_mem);
		limitKnob.resize(60, 60);
		limit.setControl(&limitKnob);
		layout.addItem(&limit);

		reload_button.setText("Reload");
		reload_button.resize(100, 50);
		layout.addItem(&reload_button);
	}

	HBoxLayout layout{this};

	LabeledControl streamer{this, "Streaming"};
	LabeledControl limit{this, "MB Limit"};

	CheckBox streamerCheck{&streamer};
	Knob limitKnob{&limit};
	Button reload_button{this};
};

DGWindow::DGWindow(void* native_window, Config& config, Settings& settings,
                   SettingsNotifier& settings_notifier)
	: Window(native_window)
	, config(config)
	, settings(settings)
	, settings_notifier(settings_notifier)
{
	int vlineSpacing = 16;

	constexpr std::size_t width = 370 + 40;
	constexpr std::size_t height = 440;
	constexpr std::size_t border = 60;

	resize(width, height);
	setCaption("DrumGizmo v" VERSION);

	layout.setResizeChildren(false);
	layout.setHAlignment(HAlignment::center);
//	layout.setSpacing(0);

	auto headerCaption = new Label(this);
	headerCaption->setText("DrumGizmo");
	headerCaption->setAlignment(TextAlignment::center);
	headerCaption->resize(width - border, 32);
	layout.addItem(headerCaption);

	auto headerLine = new VerticalLine(this);
	headerLine->resize(width - border, vlineSpacing);
	layout.addItem(headerLine);

	auto drumkitCaption = new Label(this);
	drumkitCaption->setText("Drumkit file:");
	drumkitCaption->resize(width - border, 15);
	layout.addItem(drumkitCaption);

	auto drumkitFile = new File(this);
	drumkitFile->resize(width - border, 37);
	lineedit = &drumkitFile->lineedit;
	CONNECT(&drumkitFile->browseButton, clickNotifier,
	        this, &DGWindow::kitBrowseClick);
	layout.addItem(drumkitFile);

	drumkitFileProgress = new ProgressBar(this);
	drumkitFileProgress->resize(width - border, 11);
	layout.addItem(drumkitFileProgress);

	VerticalLine *l = new VerticalLine(this);
	l->resize(width - border, vlineSpacing);
	layout.addItem(l);

	auto midimapCaption = new Label(this);
	midimapCaption->setText("Midimap file:");
	midimapCaption->resize(width - border, 15);
	layout.addItem(midimapCaption);

	auto midimapFile = new File(this);
	midimapFile->resize(width - border, 37);
	lineedit2 = &midimapFile->lineedit;
	CONNECT(&midimapFile->browseButton, clickNotifier,
	        this, &DGWindow::midimapBrowseClick);
	layout.addItem(midimapFile);

	midimapFileProgress = new ProgressBar(this);
	midimapFileProgress->resize(width - border, 11);
	midimapFileProgress->setTotal(2);
	layout.addItem(midimapFileProgress);

	VerticalLine *l2 = new VerticalLine(this);
	l2->resize(width - border, vlineSpacing);
	layout.addItem(l2);

	HumanizeControls* humanizeControls = new HumanizeControls(this);
	humanizeControls->resize(80 * 3, 80);
	layout.addItem(humanizeControls);
	CONNECT(&humanizeControls->velocityCheck, stateChangedNotifier,
	        this, &DGWindow::velocityCheckClick);

	CONNECT(&humanizeControls->attackKnob, valueChangedNotifier,
	        this, &DGWindow::attackValueChanged);

	CONNECT(&humanizeControls->falloffKnob, valueChangedNotifier,
	        this, &DGWindow::falloffValueChanged);

	// Store pointers for PluginGUI access:
	velocityCheck = &humanizeControls->velocityCheck;
	attackKnob = &humanizeControls->attackKnob;
	falloffKnob = &humanizeControls->falloffKnob;

	VerticalLine *l3 = new VerticalLine(this);
	l3->resize(width - border, vlineSpacing);
	layout.addItem(l3);

	disk_streaming_controls = new DiskStreamingControls(this);
	disk_streaming_controls->resize(80 * 3, 80);
	layout.addItem(disk_streaming_controls);
	CONNECT(&disk_streaming_controls->streamerCheck, stateChangedNotifier,
	        this, &DGWindow::streamerCheckClick);

	CONNECT(&disk_streaming_controls->limitKnob, valueChangedNotifier,
	        this, &DGWindow::limitValueChanged);

	CONNECT(&disk_streaming_controls->reload_button, clickNotifier,
	        this, &DGWindow::reloadClicked);

	VerticalLine *l4 = new VerticalLine(this);
	l4->resize(width - border, vlineSpacing);
	layout.addItem(l4);

	Label *lbl_version = new Label(this);
	lbl_version->setText(".::.  v" VERSION "  .::.  http://www.drumgizmo.org  .::.  LGPLv3 .::.");
	lbl_version->resize(width, 20);
	lbl_version->setAlignment(TextAlignment::center);
	layout.addItem(lbl_version);

	// Create file browser
	fileBrowser = new FileBrowser(this);
	fileBrowser->move(0, 0);
	fileBrowser->resize(width, height);
	fileBrowser->hide();

	CONNECT(this, settings_notifier.drumkit_file,
	        lineedit, &LineEdit::setText);
	CONNECT(this, settings_notifier.drumkit_load_status,
	        this, &DGWindow::setDrumKitLoadStatus);

	CONNECT(this, settings_notifier.midimap_file,
	        lineedit2, &LineEdit::setText);
	CONNECT(this, settings_notifier.midimap_load_status,
	        this, &DGWindow::setMidiMapLoadStatus);

	CONNECT(this, settings_notifier.enable_velocity_modifier,
	        velocityCheck, &CheckBox::setChecked);

	CONNECT(this, settings_notifier.velocity_modifier_falloff,
	        falloffKnob, &Knob::setValue);
	CONNECT(this, settings_notifier.velocity_modifier_weight,
	        attackKnob, &Knob::setValue);

	CONNECT(this, settings_notifier.number_of_files,
	        drumkitFileProgress, &ProgressBar::setTotal);

	CONNECT(this, settings_notifier.number_of_files_loaded,
	        drumkitFileProgress, &ProgressBar::setValue);

	CONNECT(this, settings_notifier.disk_cache_enable,
	        &disk_streaming_controls->streamerCheck, &CheckBox::setChecked);
	CONNECT(this, settings_notifier.disk_cache_upper_limit,
	        this, &DGWindow::limitSettingsValueChanged);
}

DGWindow::~DGWindow()
{
	delete disk_streaming_controls;
}

void DGWindow::setDrumKitLoadStatus(LoadStatus load_status)
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
	drumkitFileProgress->setState(state);
}

void DGWindow::setMidiMapLoadStatus(LoadStatus load_status)
{
	ProgressBarState state = ProgressBarState::Blue;
	switch(load_status)
	{
	case LoadStatus::Idle:
		midimapFileProgress->setValue(0);
		break;
	case LoadStatus::Loading:
		midimapFileProgress->setValue(1);
		state = ProgressBarState::Blue;
		break;
	case LoadStatus::Done:
		midimapFileProgress->setValue(2);
		state = ProgressBarState::Green;
		break;
	case LoadStatus::Error:
		midimapFileProgress->setValue(2);
		state = ProgressBarState::Red;
		break;
	}

	midimapFileProgress->setState(state);
}

void DGWindow::repaintEvent(RepaintEvent* repaintEvent)
{
	if(!visible())
	{
		return;
	}

	Painter p(*this);
	p.drawImageStretched(0,0, back, width(), height());
	p.drawImage(width() - logo.width(), height() - logo.height(), logo);
	sidebar.setSize(16, height());
	p.drawImage(0, 0, sidebar);
	p.drawImage(width() - 16, 0, sidebar);
}

void DGWindow::streamerCheckClick(bool value)
{
	settings.disk_cache_enable.store(value);
}

void DGWindow::limitValueChanged(float value)
{
	// value is in MB
	settings.disk_cache_upper_limit.store(value * 1024 * 1024);
}

void DGWindow::limitSettingsValueChanged(float value)
{
	disk_streaming_controls->limitKnob.setValue(value / (1024 * 1024));
}

void DGWindow::reloadClicked()
{
	settings.reload_counter++;
}

void DGWindow::attackValueChanged(float value)
{
	settings.velocity_modifier_weight.store(value);

#ifdef STANDALONE // For GUI debugging
	int i = value * 4;
	switch(i) {
	case 0:
		drumkitFileProgress->setState(ProgressBarState::Off);
		break;
	case 1:
		drumkitFileProgress->setState(ProgressBarState::Blue);
		break;
	case 2:
		drumkitFileProgress->setState(ProgressBarState::Green);
		break;
	case 3:
		drumkitFileProgress->setState(ProgressBarState::Red);
		break;
	default:
		break;
	}
#endif
}

void DGWindow::falloffValueChanged(float value)
{
	settings.velocity_modifier_falloff.store(value);

#ifdef STANDALONE // For GUI debugging
	drumkitFileProgress->setTotal(100);
	drumkitFileProgress->setValue(value * 100);
#endif
}

void DGWindow::velocityCheckClick(bool checked)
{
	settings.enable_velocity_modifier.store(checked);
}

void DGWindow::kitBrowseClick()
{
	std::string path = lineedit->text();
	if(path == "")
	{
		path = config.lastkit;
	}

	if(path == "")
	{
		path = lineedit2->text();
	}

	fileBrowser->setPath(path);
	CONNECT(fileBrowser, fileSelectNotifier, this, &DGWindow::selectKitFile);
	fileBrowser->show();
}

void DGWindow::midimapBrowseClick()
{
	std::string path = lineedit2->text();
	if(path == "")
	{
		path = config.lastmidimap;
	}

	if(path == "")
	{
		path = lineedit->text();
	}

	fileBrowser->setPath(path);
	CONNECT(fileBrowser, fileSelectNotifier, this, &DGWindow::selectMapFile);
	fileBrowser->show();
}

void DGWindow::selectKitFile(const std::string& filename)
{
	lineedit->setText(filename);

	fileBrowser->hide();

	std::string drumkit = lineedit->text();

	config.lastkit = drumkit;
	config.save();

	settings.drumkit_file.store(drumkit);
}

void DGWindow::selectMapFile(const std::string& filename)
{
	lineedit2->setText(filename);
	fileBrowser->hide();

	std::string midimap = lineedit2->text();

	config.lastmidimap = midimap;
	config.save();

	settings.midimap_file.store(midimap);
}


} // GUI::

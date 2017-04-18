/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            mainwindow.cc
 *
 *  Sat Nov 26 14:27:29 CET 2016
 *  Copyright 2016 Bent Bisballe Nyeng
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
#include "mainwindow.h"

#include <config.h>
#include <version.h>

#include "painter.h"

#include <string>

namespace GUI
{

MainWindow::MainWindow(Settings& settings, void* native_window)
	: Window(native_window)
	, settings_notifier(settings)
	, main_tab(this, settings, settings_notifier, config)
{
	config.load();

	CONNECT(this, sizeChangeNotifier, this, &MainWindow::sizeChanged);
	CONNECT(eventHandler(), closeNotifier, this, &MainWindow::closeEventHandler);

	setCaption("DrumGizmo v" VERSION);

	tabs.move(16, 0); // x-offset to make room for the left side bar.

	tabs.addTab("Main", &main_tab);
	tabs.addTab("About", &about_text_field);

	about_text_field.setText(getAboutText());
	about_text_field.setReadOnly(true);
}

MainWindow::~MainWindow()
{
	config.save();
}

bool MainWindow::processEvents()
{
//	if(!initialised)
//	{
//		return running;
//	}

	settings_notifier.evaluate();
	eventHandler()->processEvents();

	if(closing)
	{
		closeNotifier();
		closing = false;
		return false;
	}

	return true;
}

void MainWindow::repaintEvent(RepaintEvent* repaintEvent)
{
	if(!visible())
	{
		return;
	}

	Painter painter(*this);

	auto bar_height = tabs.getBarHeight();

	// Grey background
	painter.drawImageStretched(0, 0, back, width(), height());

	// DrumGizmo logo
	painter.drawImage(width() - logo.width() - 16,
	                  height() - logo.height() - 10, logo);

	// DrumGizmo version
	std::string version_string("v." + std::string(VERSION));
	auto version_x = width() - font.textWidth(version_string) - sidebar.width() - 5;
	auto version_y = height() - 5;
	painter.setColour(Colour(0.18));
	painter.drawText(version_x, version_y, font, version_string);

	// Topbar above the sidebars
	topbar.setSize(16, bar_height);
	painter.drawImage(0, 0, topbar);
	painter.drawImage(width() - 16, 0, topbar);

	// Sidebars
	sidebar.setSize(16, height() - bar_height + 1);
	painter.drawImage(0, bar_height-1, sidebar);
	painter.drawImage(width() - 16, bar_height-1, sidebar);
}

void MainWindow::sizeChanged(std::size_t width, std::size_t height)
{
	tabs.resize(width - 2 * 16, height);
}

void MainWindow::closeEventHandler()
{
	closing = true;
}

std::string MainWindow::getAboutText()
{
	std::string about_text;

	// About
	about_text.append(
	"=============\n"
	"             About\n"
	"=============\n"
	"\n");
	about_text.append(about.data());

	// Version
	about_text.append(
	"\n"
	"\n"
	"=============\n"
	"            Version\n"
	"=============\n"
	"\n");
	about_text.append(std::string(VERSION) + "\n");

	// Authors
	about_text.append(
	"\n"
	"\n"
	"=============\n"
	"            Authors\n"
	"=============\n"
	"\n");
	about_text.append(authors.data());

	// GPL
	about_text.append(
	"\n"
	"\n"
	"=============\n"
	"            License\n"
	"=============\n"
	"\n");
	about_text.append(gpl.data());

	return about_text;
}

} // GUI::

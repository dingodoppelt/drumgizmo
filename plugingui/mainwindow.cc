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

#include "painter.h"

namespace GUI
{

MainWindow::MainWindow(Settings& settings, void* native_window)
	: Window(native_window)
	, settings(settings)
{
	CONNECT(this, sizeChangeNotifier, this, &MainWindow::sizeChanged);
	CONNECT(eventHandler(), closeNotifier, this, &MainWindow::closeEventHandler);
	tabs.move(16, 0); // x-offset to make room for the left side bar.
	setCaption("DrumGizmo v" VERSION);
}

bool MainWindow::processEvents()
{
//	if(!initialised)
//	{
//		return running;
//	}

	eventHandler()->processEvents();

	{
		Painter p(*this);
		settings_notifier.evaluate();
	}

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

	// Grey background
	painter.drawImageStretched(0, 0, back, width(), height());

	// DrumGizmo logo
	painter.drawImage(width() - logo.width() - 16,
	                  height() - logo.height(), logo);

	// Sidebars
	sidebar.setSize(16, height());
	painter.drawImage(0, 0, sidebar);
	painter.drawImage(width() - 16, 0, sidebar);
}

void MainWindow::sizeChanged(std::size_t width, std::size_t height)
{
	tabs.resize(width - 2 * 16, height);
}

void MainWindow::closeEventHandler()
{
	closing = true;
}

} // GUI::

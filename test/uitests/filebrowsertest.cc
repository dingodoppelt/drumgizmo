/* -*- Mode: c++ -*- */
/***************************************************************************
 *            filebrowsertest.cc
 *
 *  Sat Apr 15 22:24:06 CEST 2017
 *  Copyright 2017 Bent Bisballe Nyeng
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
#include <iostream>
#include <chrono>
#include <thread>

#include <hugin.hpp>

#include <dggui/window.h>
#include <dggui/button.h>
#include <dggui/label.h>
#include <filebrowser.h>

class TestWindow
	: public GUI::Window
{
public:
	TestWindow()
		: GUI::Window(nullptr)
	{
		setCaption("FileBrowserTest Window");
		CONNECT(eventHandler(), closeNotifier,
		        this, &TestWindow::closeEventHandler);

		CONNECT(file_browser.eventHandler(), closeNotifier,
		        this, &TestWindow::dialogCloseEventHandler);

		CONNECT(&file_browser, fileSelectNotifier,
		        this, &TestWindow::fileSelected);

		CONNECT(&file_browser, fileSelectCancelNotifier,
		        this, &TestWindow::fileCanceled);


		button.move(0, 0);
		button.resize(120, 30);
		button.setText("Browse...");

		label.move(0, 40);
		label.resize(120, 30);
		label.setText("Filename will be shown here.");

		CONNECT(&button, clickNotifier,
		        this, &TestWindow::dialogShow);
	}

	void fileSelected(const std::string& filename)
	{
		label.setText(filename);
	}

	void fileCanceled()
	{
		label.setText("[Canceled]");
	}

	void closeEventHandler()
	{
		closing = true;
	}

	void dialogCloseEventHandler()
	{
		file_browser.hide();
	}

	void dialogShow()
	{
		file_browser.show();
		file_browser.resize(300, 300);
	}

	bool processEvents()
	{
		eventHandler()->processEvents();
		return !closing;
	}

	void repaintEvent(GUI::RepaintEvent* repaintEvent)
	{
		GUI::Painter painter(*this);

		label.resize(width(), 30);

		//painter.clear();
		painter.setColour(GUI::Colour(0,1,0));
		painter.drawFilledRectangle(0, 0, width(), height());
	}

private:
	bool closing{false};

	GUI::Button button{this};
	GUI::Label label{this};

	GUI::FileBrowser file_browser{this};
};

int main()
{
	INFO(example, "We are up and running");

	TestWindow test_window;
	test_window.show();
	test_window.resize(300,300);

	while(test_window.processEvents())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	return 0;
}

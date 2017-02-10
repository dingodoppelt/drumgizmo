/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            resizetest.cc
 *
 *  Sun Feb  5 20:05:24 CET 2017
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

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <hugin.hpp>
#include <window.h>
#include <font.h>
#include <painter.h>

class TestWindow
	: public GUI::Window
{
public:
	TestWindow()
		: GUI::Window(nullptr)
	{
		setCaption("ResizeTest Window");
		CONNECT(eventHandler(), closeNotifier,
		        this, &TestWindow::closeEventHandler);
		CONNECT(this, sizeChangeNotifier, this, &TestWindow::sizeChanged);
		CONNECT(this, positionChangeNotifier, this, &TestWindow::positionChanged);
	}

	void sizeChanged(std::size_t width, std::size_t height)
	{
		reportedSize = std::make_pair(width, height);
		repaintEvent(nullptr);
	}

	void positionChanged(int x, int y)
	{
		reportedPosition = std::make_pair(x, y);
		repaintEvent(nullptr);
	}

	void closeEventHandler()
	{
		closing = true;
	}

	bool processEvents()
	{
		eventHandler()->processEvents();
		return !closing;
	}

	void repaintEvent(GUI::RepaintEvent* repaintEvent)
	{
		GUI::Painter painter(*this);

		//painter.clear();
		painter.setColour(GUI::Colour(0,1,0));
		painter.drawFilledRectangle(0, 0, width(), height());

		auto currentSize = std::make_pair(width(), height());
		auto currentPosition = std::make_pair(x(), y());

		{
			painter.setColour(GUI::Colour(1,0,0));
			char str[64];
			sprintf(str, "reported: (%d, %d); (%d, %d)",
			        (int)reportedPosition.first,
			        (int)reportedPosition.second,
			        (int)reportedSize.first,
			        (int)reportedSize.second);
			auto stringWidth = font.textWidth(str);
			auto stringHeight = font.textHeight(str);
			painter.drawText(reportedSize.first / 2 - stringWidth / 2,
			                 reportedSize.second / 2 + stringHeight / 2 - 7,
			                 font, str, false);
		}

		{
			painter.setColour(GUI::Colour(1,0,0));
			char str[64];
			sprintf(str, "current: (%d, %d); (%d, %d)",
			        (int)currentPosition.first,
			        (int)currentPosition.second,
			        (int)currentSize.first,
			        (int)currentSize.second);
			auto stringWidth = font.textWidth(str);
			auto stringHeight = font.textHeight(str);
			painter.drawText(currentSize.first / 2 - stringWidth / 2,
			                 currentSize.second / 2 + stringHeight / 2 + 7,
			                 font, str, false);
		}
	}

private:
	bool closing{false};
	GUI::Font font{":font.png"};
	std::pair<std::size_t, std::size_t> reportedSize;
	std::pair<int, int> reportedPosition;
};

int main()
{
	INFO(example, "We are up and running");

	TestWindow test_window;
	test_window.show();
	test_window.resize(300,300);

	while(test_window.processEvents())
	{
#ifdef WIN32
		SleepEx(50, FALSE);
#else
		usleep(50000);
#endif
	}

	return 0;
}

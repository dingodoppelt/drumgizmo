/* -*- Mode: c++ -*- */
/***************************************************************************
 *            powerwidgettest.cc
 *
 *  Fri Apr 24 17:26:30 CEST 2020
 *  Copyright 2020 Bent Bisballe Nyeng
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

#include <button.h>
#include <checkbox.h>
#include <frame.h>
#include <hugin.hpp>
#include <label.h>
#include <layout.h>
#include <window.h>
#include <painter.h>
#include <settings.h>
#include <powerwidget.h>

class TestWindow
	: public GUI::Window
{
public:
	TestWindow()
		: GUI::Window()
	{
		setCaption("PowerWidgetTest Window");
		CONNECT(eventHandler(), closeNotifier, this,
		        &TestWindow::closeEventHandler);
		CONNECT(this, sizeChangeNotifier, this, &TestWindow::sizeChanged);
	}

	void sizeChanged(std::size_t width, std::size_t height)
	{
		w.resize(width, height);
	}

	void closeEventHandler()
	{
		closing = true;
	}

	bool processEvents()
	{
		settings_notifier.evaluate();
		eventHandler()->processEvents();
//		static unsigned int cnt = 500;
//		++cnt;
//
//		bool a = cnt / 1000 % 2 == 0;
//		float b = (((cnt * 6) % 300) +   0) / 1000.0;
//		float c = (((cnt * 5) % 300) +   0) / 1000.0;
//		float d = (((cnt * 4) % 300) + 300) / 1000.0;
//		float e = (((cnt * 3) % 300) + 300) / 1000.0;
//		float f = (((cnt * 2) % 300) + 600) / 1000.0;
//		float g = (((cnt * 1) % 300) + 600) / 1000.0;
//		bool h = cnt / 400 % 2 == 0;
//
//		printf("cnt: % 4d: [ %s (%f %f) (%f %f) (%f %f) %s ]\n",
//		       cnt, a?"true":"false", b, c, d, e, f, g, h?"true":"false");
//
//		settings.enable_powermap.store(a);
//		settings.fixed0_x.store(b);
//		settings.fixed0_y.store(c);
//		settings.fixed1_x.store(d);
//		settings.fixed1_y.store(e);
//		settings.fixed2_x.store(f);
//		settings.fixed2_y.store(g);
//		settings.shelf.store(h);

		return !closing;
	}

	void repaintEvent(GUI::RepaintEvent* repaintEvent) override
	{
		GUI::Painter painter(*this);
		painter.setColour(GUI::Colour(0.85));
		painter.drawFilledRectangle(0, 0, width() - 1, height() - 1);
	}

private:
	bool closing{false};

	Settings settings;
	SettingsNotifier settings_notifier{settings};
	PowerWidget w{this, settings, settings_notifier};
};

int main()
{
	INFO(example, "We are up and running");

	TestWindow test_window;
	test_window.show();
	test_window.resize(300, 300);

	while(test_window.processEvents())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}

	return 0;
}

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            tabwidgettest.cc
 *
 *  Thu Feb  9 19:20:06 CET 2017
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

#include <platform.h>

#if DG_PLATFORM == DG_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <unistd.h>

#include <hugin.hpp>
#include <window.h>
#include <tabwidget.h>
#include <label.h>
#include <layout.h>
#include <button.h>

class Widget1
	: public GUI::Widget
{
public:
	Widget1(GUI::Widget* parent)
		: GUI::Widget(parent)
	{
		layout.setResizeChildren(true);
		layout.setHAlignment(GUI::HAlignment::center);

		label1.setText("Label1");
		label1.setAlignment(GUI::TextAlignment::left);
		layout.addItem(&label1);

		label2.setText("Label2");
		label2.setAlignment(GUI::TextAlignment::center);
		layout.addItem(&label2);

		label3.setText("Label3");
		label3.setAlignment(GUI::TextAlignment::right);
		layout.addItem(&label3);
	}

private:
	GUI::VBoxLayout layout{this};
	GUI::Label label1{this};
	GUI::Label label2{this};
	GUI::Label label3{this};
};

class Widget2
	: public GUI::Button
{
public:
	Widget2(GUI::Widget* parent)
		: GUI::Button(parent)
	{
		setText("Widget2");
	}
};

class Widget3
	: public GUI::Widget
{
public:
	Widget3(GUI::Widget* parent)
		: GUI::Widget(parent)
	{
		label.move(100, 80);
		label.resize(100, 20);
		label.setText("Widget3");
	}
private:
	GUI::Label label{this};
};

class TestWindow
	: public GUI::Window
{
public:
	TestWindow()
		: GUI::Window(nullptr)
	{
		setCaption("TabWidgetTest Window");
		CONNECT(eventHandler(), closeNotifier,
		        this, &TestWindow::closeEventHandler);
		CONNECT(this, sizeChangeNotifier, this, &TestWindow::sizeChanged);
		tabs.move(0, 0);
		tabs.addTab("Tab1", &widget1);
		tabs.addTab("Tab2", &widget2);
		tabs.addTab("Tab3", &widget3);
}

	void sizeChanged(std::size_t width, std::size_t height)
	{
		tabs.resize(this->width(), this->height());
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

	void repaintEvent(GUI::RepaintEvent* repaintEvent) override
	{
		GUI::Painter painter(*this);
		painter.setColour(GUI::Colour(0.85));
		painter.drawFilledRectangle(0, 0, width() - 1, height() - 1);
	}

private:
	bool closing{false};
	GUI::TabWidget tabs{this};
	Widget1 widget1{this};
	Widget2 widget2{this};
	Widget3 widget3{this};
};

int main()
{
	INFO(example, "We are up and running");

	TestWindow test_window;
	test_window.show();
	test_window.resize(300,300);

	while(test_window.processEvents())
	{
#if DG_PLATFORM == DG_PLATFORM_WINDOWS
		SleepEx(50, FALSE);
#else
		usleep(50000);
#endif
	}

	return 0;
}

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            framewidgettest.cc
 *
 *  Sat Feb 11 23:36:04 CET 2017
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
#include <iostream>
#include <chrono>
#include <thread>

#include <hugin.hpp>

#include <dggui/button.h>
#include <dggui/checkbox.h>
#include <dggui/frame.h>
#include <dggui/label.h>
#include <dggui/layout.h>
#include <dggui/window.h>
#include <dggui/painter.h>

// TODO: fix segfault on close

class Widget1 : public dggui::Widget
{
public:
	Widget1(dggui::Widget* parent) : dggui::Widget(parent)
	{
		layout.setResizeChildren(true);
		layout.setHAlignment(dggui::HAlignment::center);

		label1.setText("Label1");
		label1.setAlignment(dggui::TextAlignment::left);
		layout.addItem(&label1);

		label2.setText("Label2");
		label2.setAlignment(dggui::TextAlignment::center);
		layout.addItem(&label2);

		label3.setText("Label3");
		label3.setAlignment(dggui::TextAlignment::right);
		layout.addItem(&label3);
	}

private:
	dggui::VBoxLayout layout{this};
	dggui::Label label1{this};
	dggui::Label label2{this};
	dggui::Label label3{this};
};

class Widget2 : public dggui::Widget
{
public:
	Widget2(dggui::Widget* parent) : dggui::Widget(parent)
	{
		layout.setSpacing(10);
		layout.setResizeChildren(true);
		layout.setVAlignment(dggui::VAlignment::top);

		label1.setText("Label1");
		label1.setAlignment(dggui::TextAlignment::right);
		layout.addItem(&label1);

		label2.setText("Label2");
		label2.setAlignment(dggui::TextAlignment::left);
		layout.addItem(&label2);

		layout.addItem(&checkbox);
	}

private:
	dggui::HBoxLayout layout{this};
	dggui::Label label1{this};
	dggui::Label label2{this};
	dggui::CheckBox checkbox{this};
};

class TestWindow : public dggui::Window
{
public:
	TestWindow() : dggui::Window(nullptr)
	{
		setCaption("FrameWidgetTest Window");
		CONNECT(eventHandler(), closeNotifier, this,
		    &TestWindow::closeEventHandler);
		CONNECT(this, sizeChangeNotifier, this, &TestWindow::sizeChanged);

		layout.setSpacing(10);
		layout.setResizeChildren(true);
		layout.setHAlignment(dggui::HAlignment::center);

		layout.addItem(&frame1);
		layout.addItem(&frame2);
		layout.addItem(&frame3);
		layout.addItem(&frame4);

		frame1.setTitle("Frame 1 Title");
		frame2.setTitle("Frame 2 Title");
		frame3.setTitle("Frame 3 Title");
		frame4.setTitle("Frame 4 Title");

		frame1.setContent(&w1_1);
		frame2.setContent(&w1_2);
		frame3.setContent(&w2_1);
		frame4.setContent(&w2_2);
	}

	void sizeChanged(std::size_t width, std::size_t height)
	{
		// TODO: Do we have to do anything here?
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

	void repaintEvent(dggui::RepaintEvent* repaintEvent) override
	{
		dggui::Painter painter(*this);
		painter.setColour(dggui::Colour(0.85));
		painter.drawFilledRectangle(0, 0, width() - 1, height() - 1);
	}

private:
	bool closing{false};

	dggui::VBoxLayout layout{this};

	dggui::FrameWidget frame1{this, true};
	dggui::FrameWidget frame2{this, false};
	dggui::FrameWidget frame3{this, true};
	dggui::FrameWidget frame4{this, false};

	Widget1 w1_1{this};
	Widget1 w1_2{this};

	Widget2 w2_1{this};
	Widget2 w2_2{this};
};

int main()
{
	INFO(example, "We are up and running");

	TestWindow test_window;
	test_window.show();
	test_window.resize(300, 300);

	while(test_window.processEvents())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	return 0;
}

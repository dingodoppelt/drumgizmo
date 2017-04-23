/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            frame.h
 *
 *  Tue Feb  7 21:07:56 CET 2017
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
#pragma once

#include <notifier.h>

#include "font.h"
#include "powerbutton.h"
#include "widget.h"

namespace GUI
{

class FrameWidget
	: public Widget
{
public:
	FrameWidget(Widget* parent, bool has_switch = false);
	virtual ~FrameWidget() = default;

	// From Widget:
	virtual bool isFocusable() override { return false; }
	virtual bool catchMouse() override { return false; }

	bool isSwitchedOn() { return is_switched_on; }

	void setTitle(std::string const& title);
	void setContent(Widget* content);

	void setOnSwitch(bool on);

	Notifier<bool> onSwitchChangeNotifier; // (bool on)

protected:
	// From Widget:
	virtual void repaintEvent(RepaintEvent* repaintEvent) override;

	//! Callback for Widget::sizeChangeNotifier
	void sizeChanged(int width, int height);

private:
	//
	// upper bar
	//

	// label
	Font font;
	std::string title;
	GUI::Colour label_colour{0.1};
	std::size_t label_width;

	// switch
	bool is_switched_on;
	PowerButton power_button{this};

	void powerButtonStateChanged(bool clicked);

	// grey box
	std::size_t bar_height;
	GUI::Colour grey_box_colour{0.7};
	GUI::Colour background_colour{0.85, 0.8};

	//
	// content
	//

	// content frame
	GUI::Colour frame_colour_top{0.95};
	GUI::Colour frame_colour_bottom{0.4};
	GUI::Colour frame_colour_side{0.6};

	// content box
	Widget* content{nullptr};
	std::size_t content_margin{12};

	std::size_t content_start_x;
	std::size_t content_start_y;
	std::size_t content_width;
	std::size_t content_height;
};

} // GUI::

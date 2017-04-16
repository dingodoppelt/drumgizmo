/* -*- Mode: c++ -*- */
/***************************************************************************
 *            button_base.cc
 *
 *  Sat Apr 15 21:45:30 CEST 2017
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
#include "button_base.h"

namespace GUI {

ButtonBase::ButtonBase(Widget *parent)
	: Widget(parent)
	, draw_state(State::Up)
	, button_state(State::Up)
{
}

ButtonBase::~ButtonBase()
{
}

void ButtonBase::buttonEvent(ButtonEvent* buttonEvent)
{
	// Ignore everything except left clicks.
	if(buttonEvent->button != MouseButton::left)
	{
		return;
	}

	if(buttonEvent->direction == Direction::down)
	{
		draw_state = State::Down;
		button_state = State::Down;
		in_button = true;
		redraw();
	}

	if(buttonEvent->direction == Direction::up)
	{
		draw_state = State::Up;
		button_state = State::Up;
		redraw();
		if(in_button)
		{
			clicked();
			clickNotifier();
		}
	}
}

void ButtonBase::setText(const std::string& text)
{
	this->text = text;
	redraw();
}

void ButtonBase::mouseLeaveEvent()
{
	in_button = false;
	if(button_state == State::Down)
	{
		draw_state = State::Up;
		redraw();
	}
}

void ButtonBase::mouseEnterEvent()
{
	in_button = true;
	if(button_state == State::Down)
	{
		draw_state = State::Down;
		redraw();
	}
}

} // GUI::

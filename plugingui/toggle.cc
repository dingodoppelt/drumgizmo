/* -*- Mode: c++ -*- */
/***************************************************************************
 *            toggle.cc
 *
 *  Wed Mar 22 22:58:57 CET 2017
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
#include "toggle.h"

namespace GUI
{

Toggle::Toggle(Widget* parent) : Widget(parent)
{
}

void Toggle::buttonEvent(ButtonEvent* buttonEvent)
{
	// Ignore everything except left clicks.
	if(buttonEvent->button != MouseButton::left)
	{
		return;
	}

	if((buttonEvent->direction == Direction::up) || buttonEvent->doubleClick)
	{
		buttonDown = false;
		clicked = false;
		if(inCheckbox)
		{
			internalSetChecked(!state);
		}
	}
	else
	{
		buttonDown = true;
		clicked = true;
	}

	redraw();
}

void Toggle::setText(std::string text)
{
	_text = text;
	redraw();
}

void Toggle::keyEvent(KeyEvent* keyEvent)
{
	if(keyEvent->keycode == Key::character && keyEvent->text == " ")
	{
		if(keyEvent->direction == Direction::up)
		{
			clicked = false;
			internalSetChecked(!state);
		}
		else
		{
			clicked = true;
		}

		redraw();
	}
}

bool Toggle::checked()
{
	return state;
}

void Toggle::setChecked(bool c)
{
	internalSetChecked(c);
}

void Toggle::mouseLeaveEvent()
{
	inCheckbox = false;
	if(buttonDown)
	{
		clicked = false;
		redraw();
	}
}

void Toggle::mouseEnterEvent()
{
	inCheckbox = true;
	if(buttonDown)
	{
		clicked = true;
		redraw();
	}
}

void Toggle::internalSetChecked(bool checked)
{
	if(state == checked)
	{
		return;
	}

	state = checked;
	stateChangedNotifier(state);
	redraw();
}

} // GUI::
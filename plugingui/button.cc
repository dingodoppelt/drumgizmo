/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            button.cc
 *
 *  Sun Oct  9 13:01:56 CEST 2011
 *  Copyright 2011 Bent Bisballe Nyeng
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
#include "button.h"

#include "painter.h"

#include <stdio.h>
#include <hugin.hpp>

namespace GUI {

Button::Button(Widget *parent)
	: Widget(parent)
	, draw_state(up)
	, button_state(up)
{
}

Button::~Button()
{
}

void Button::buttonEvent(ButtonEvent* buttonEvent)
{
	// Ignore everything except left clicks.
	if(buttonEvent->button != MouseButton::left)
	{
		return;
	}

	if(buttonEvent->direction == Direction::down)
	{
		draw_state = down;
		button_state = down;
		in_button = true;
		repaintEvent(nullptr);
	}

	if(buttonEvent->direction == Direction::up)
	{
		draw_state = up;
		button_state = up;
		repaintEvent(nullptr);
		if(in_button)
		{
			clicked();
			clickNotifier();
		}
	}
}

void Button::repaintEvent(RepaintEvent* repaintEvent)
{
	Painter p(*this);

	p.clear();

	int padTop = 3;
	int padLeft = 0;

	int w = width();
	int h = height();
	if(w == 0 || h == 0)
	{
		return;
	}

	switch(draw_state) {
	case up:
		box_up.setSize(w - padLeft, h - padTop);
		p.drawImage(padLeft, padTop, box_up);

		break;
	case down:
		box_down.setSize(w - padLeft, h - padTop);
		p.drawImage(padLeft, padTop, box_down);
		break;
	}

	p.setColour(Colour(0.1));
	p.drawText(width()/2-(text.length()*3)+(draw_state==up?0:1) + (padLeft / 2),
	           height()/2+5+1+(draw_state==up?0:1) + (padTop / 2), font, text,
	           true);
}

void Button::setText(const std::string& text)
{
	this->text = text;
	repaintEvent(nullptr);
}

void Button::mouseLeaveEvent()
{
	in_button = false;
	if(button_state == down)
	{
		draw_state = up;
		repaintEvent(nullptr);
	}
}

void Button::mouseEnterEvent()
{
	in_button = true;
	if(button_state == down)
	{
		draw_state = down;
		repaintEvent(nullptr);
	}
}

} //GUI::

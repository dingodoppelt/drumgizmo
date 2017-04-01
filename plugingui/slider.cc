/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            slider.cc
 *
 *  Sat Nov 26 18:10:22 CET 2011
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
#include "slider.h"

#include "painter.h"

#include <hugin.hpp>
#include <stdio.h>

namespace GUI
{

Slider::Slider(Widget* parent) : Widget(parent)
{
	state = State::up;

	currentValue = 0.0;
	maximum = 1.0;
	minimum = 0.0;
}

void Slider::setValue(float newValue)
{
	currentValue = newValue;
	redraw();
	clickNotifier();
}

float Slider::value()
{
	return currentValue;
}

void Slider::repaintEvent(RepaintEvent* repaintEvent)
{
	Painter p(*this);

	float alpha = 0.8;

	int xpos = (int)((currentValue / maximum) * (float)(width() - 1));

	if(hasKeyboardFocus())
	{
		p.setColour(Colour(0.6, alpha));
	}
	else
	{
		p.setColour(Colour(0.5, alpha));
	}

	p.drawFilledRectangle(0, 0, width(), height());

	// p.setColour(Colour(0.1, alpha));
	// p.drawRectangle(0,0,width()-1,height() - 1);

	p.setColour(Colour(1, 0, 0, alpha));
	p.drawLine(xpos, 0, xpos, height() - 1);

	// p.setColour(Colour(0.8, alpha));
	// switch(state) {
	// case State::up:
	//	p.drawLine(0, 0, 0, height() - 1);
	//	p.drawLine(0, 0, width() - 1, 0);
	//	break;
	// case State::down:
	//	p.drawLine(width() - 1, 0, width() - 1, height() - 1);
	//	p.drawLine(width() - 1, height() - 1, 0, height() - 1);
	//	break;
	//}

	p.setColour(Colour(0.3, alpha));

	if(height() > 0 && width() > 0)
	{
		p.drawPoint(0, height() - 1);
		p.drawPoint(width() - 1, 0);
	}
}

void Slider::buttonEvent(ButtonEvent* buttonEvent)
{
	// Ignore everything except left clicks.
	if(buttonEvent->button != MouseButton::left)
	{
		return;
	}

	if(buttonEvent->direction == Direction::down)
	{
		state = State::down;
		currentValue = maximum / (float)width() * (float)buttonEvent->x;

		if(currentValue < 0)
		{
			currentValue = 0;
		}

		if(currentValue > 1)
		{
			currentValue = 1;
		}

		redraw();
		clickNotifier();
	}

	if(buttonEvent->direction == Direction::up)
	{
		state = State::up;
		currentValue = maximum / (float)width() * (float)buttonEvent->x;

		if(currentValue < 0)
		{
			currentValue = 0;
		}

		if(currentValue > 1)
		{
			currentValue = 1;
		}

		redraw();
		clickNotifier();
	}
}

void Slider::mouseMoveEvent(MouseMoveEvent* mouseMoveEvent)
{
	if(state == State::down)
	{
		currentValue = maximum / (float)width() * (float)mouseMoveEvent->x;

		if(currentValue < 0)
		{
			currentValue = 0;
		}

		if(currentValue > 1)
		{
			currentValue = 1;
		}

		redraw();
		clickNotifier();
	}
}

} // GUI::

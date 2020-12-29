/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            knob.cc
 *
 *  Thu Feb 28 07:37:27 CET 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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
#include "knob.h"

#include "painter.h"

#include <hugin.hpp>
#include <cmath>

namespace
{
const double pi = std::atan(1.0) * 4.0;
}

namespace GUI
{

Knob::Knob(Widget *parent)
	: Widget(parent)
	, img_knob(getImageCache(), ":resources/knob.png")
{
	state = up;

	maximum = 1.0;
	minimum = 0.0;

	current_value = 0.0;

	mouse_offset_x = 0;
}

void Knob::setValue(float value)
{
	value -= minimum;
	value /= (maximum - minimum);
	internalSetValue(value);
}

void Knob::setDefaultValue(float value)
{
	default_value = value;
}

void Knob::setRange(float minimum, float maximum)
{
	this->minimum = minimum;
	this->maximum = maximum;
	internalSetValue(current_value);
}

float Knob::value()
{
	return current_value * (maximum - minimum) + minimum;
}

void Knob::showValue(bool show_value)
{
	this->show_value = show_value;
}

void Knob::scrollEvent(ScrollEvent* scrollEvent)
{
	float value = (current_value - (scrollEvent->delta / 200.0));
	internalSetValue(value);
}

void Knob::mouseMoveEvent(MouseMoveEvent* mouseMoveEvent)
{
	if(state == down)
	{
		if(mouse_offset_x == (mouseMoveEvent->x + (-1 * mouseMoveEvent->y)))
		{
			return;
		}

		float dval =
			mouse_offset_x - (mouseMoveEvent->x + (-1 * mouseMoveEvent->y));
		float value = current_value - (dval / 300.0);

		internalSetValue(value);

		mouse_offset_x = mouseMoveEvent->x + (-1 * mouseMoveEvent->y);
	}
}

void Knob::keyEvent(KeyEvent* keyEvent)
{
	if(keyEvent->direction != Direction::up)
	{
		return;
	}

	float value = current_value;
	switch(keyEvent->keycode) {
	case Key::up:
		value += 0.01;
		break;
	case Key::down:
		value -= 0.01;
		break;
	case Key::right:
		value += 0.01;
		break;
	case Key::left:
		value -= 0.01;
		break;
	case Key::home:
		value = 0;
		break;
	case Key::end:
		value = 1;
		break;
	default:
		break;
	}

	internalSetValue(value);
}

void Knob::buttonEvent(ButtonEvent* buttonEvent)
{
	// Ignore everything except left clicks.
	if(buttonEvent->button != MouseButton::left)
	{
		return;
	}

	if(buttonEvent->doubleClick)
	{
		float value = default_value;
		value -= minimum;
		value /= (maximum - minimum);
		internalSetValue(value);
		return;
	}

	if(buttonEvent->direction == Direction::down)
	{
		state = down;
		mouse_offset_x = buttonEvent->x + (-1 * buttonEvent->y);
		return;
	}

	if(buttonEvent->direction == Direction::up)
	{
		state = up;
		mouse_offset_x = buttonEvent->x + (-1 * buttonEvent->y);
		clicked();
		return;
	}
}

void Knob::repaintEvent(RepaintEvent* repaintEvent)
{
	int diameter = (width()>height()?height():width());
	int radius = diameter / 2;
	int center_x = width() / 2;
	int center_y = height() / 2;

	Painter p(*this);
	p.clear();

	p.drawImageStretched(0, 0, img_knob, diameter, diameter);

	float range = maximum - minimum;

	if (show_value) {
		// Show 0, 1 or 2 decimal point depending on the size of the range
		char buf[64];
		if(range> 100.0f)
		{
			sprintf(buf, "%.0f", current_value * range + minimum);
		}
		else if(range > 10.0f)
		{
			sprintf(buf, "%.1f", current_value * range + minimum);
		}
		else
		{
			sprintf(buf, "%.2f", current_value * range + minimum);
		}
		p.drawText(center_x - font.textWidth(buf) / 2 + 1,
				   center_y + font.textHeight(buf) / 2 + 1, font, buf);
	}

	// Make it start from 20% and stop at 80%
	double padval = current_value * 0.8 + 0.1;

	double from_x = sin((-1 * padval + 1) * 2 * pi) * radius * 0.6;
	double from_y = cos((-1 * padval + 1) * 2 * pi) * radius * 0.6;

	double to_x = sin((-1 * padval + 1) * 2 * pi) * radius * 0.8;
	double to_y = cos((-1 * padval + 1) * 2 * pi) * radius * 0.8;

	// Draw "fat" line by drawing 9 lines with moved start/ending points.
	p.setColour(Colour(1.0f, 0.0f, 0.0f, 1.0f));
	for(int _x = -1; _x < 2; _x++)
	{
		for(int _y = -1; _y < 2; _y++)
		{
			p.drawLine(from_x + center_x + _x,
			           from_y + center_y + _y,
			           to_x + center_x + _x,
			           to_y + center_y + _y);
		}
	}
}

void Knob::internalSetValue(float new_value)
{
	if(new_value < 0.0)
	{
	  new_value = 0.0;
	}

	if(new_value > 1.0)
	{
		new_value = 1.0;
	}

	if(new_value == current_value)
	{
		return;
	}

	current_value = new_value;
	valueChangedNotifier(value());
	redraw();
}

} // GUI::

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            scrollbar.cc
 *
 *  Sun Apr 14 12:54:58 CEST 2013
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
#include "scrollbar.h"

#include <hugin.hpp>

#include "painter.h"

namespace GUI {

ScrollBar::ScrollBar(Widget *parent)
	: Widget(parent)
{
}

void ScrollBar::setRange(int r)
{
	rangeValue = r;
	setValue(value());
	repaintEvent(nullptr);
}

int ScrollBar::range()
{
	return rangeValue;
}

void ScrollBar::setMaximum(int m)
{
	maxValue = m;
	if(maxValue < rangeValue)
	{
		rangeValue = maxValue;
	}
	setValue(value());
	repaintEvent(nullptr);
}

int ScrollBar::maximum()
{
	return maxValue;
}

void ScrollBar::addValue(int delta)
{
	setValue(value() + delta);
}

void ScrollBar::setValue(int value)
{
	if(value > (maxValue - rangeValue))
	{
		value = maxValue - rangeValue;
	}

	if(value < 0)
	{
		value = 0;
	}

	if(currentValue == value)
	{
		return;
	}

	currentValue = value;

	valueChangeNotifier(value);

	repaintEvent(nullptr);
}

int ScrollBar::value()
{
	return currentValue;
}

//! Draw an up/down arrow at (x,y) with the bounding box size (w,h)
//! If h is negative the arrow will point down, if positive it will point up.
static void drawArrow(Painter &p, int x, int y, int w, int h)
{
	if(h < 0)
	{
		y -= h;
	}

	p.drawLine(x, y, x + (w / 2), y + h);
	p.drawLine(x + (w / 2), y + h, x + w, y);

	++y;
	p.drawLine(x, y, x + (w / 2), y + h);
	p.drawLine(x + (w / 2), y + h, x + w, y);
}

void ScrollBar::repaintEvent(RepaintEvent* repaintEvent)
{
	Painter p(*this);

	p.clear();

	p.drawImageStretched(0, 0, bg_img, width(), height());

	p.setColour(Colour(183.0/255.0, 219.0/255.0 , 255.0/255.0, 1));
	if(!maxValue)
	{
		return;
	}

	{
		int h = height() - 2 * width() - 3;
		int offset = width() + 2;

		int y_val1 = (currentValue * h) / maxValue;
		int y_val2 = ((currentValue + rangeValue) * h) / maxValue;

		p.drawFilledRectangle(2, y_val1 + offset, width() - 1, y_val2 + offset);
	}

	p.drawLine(0, 0, 0, height());

	drawArrow(p, width()/4, width()/4, width()/2, -1 * (width()/3));
	p.drawLine(0, width(), width(), width());

	drawArrow(p, width()/4, height() - width() + width()/4, width()/2, width()/3);
	p.drawLine(0, height() - width(), width(), height() - width());
}

void ScrollBar::scrollEvent(ScrollEvent* scrollEvent)
{
	setValue(value() + scrollEvent->delta);
}

void ScrollBar::mouseMoveEvent(MouseMoveEvent* mouseMoveEvent)
{
	if(!dragging)
	{
		return;
	}

	float delta = yOffset - mouseMoveEvent->y;

	int h = height() - 2 * width() - 3;
	delta /= (float)h / (float)maxValue;

	int newval = valueOffset - delta;
	if(newval != value())
	{
		setValue(newval);
	}
}

void ScrollBar::buttonEvent(ButtonEvent* buttonEvent)
{
	// Ignore everything except left clicks.
	if (buttonEvent->button != MouseButton::left) {
		return;
	}

	if((buttonEvent->y < (int)width()) && buttonEvent->y > 0)
	{
		if(buttonEvent->direction == Direction::down)
		{
			addValue(-1);
		}

		return;
	}

	if((buttonEvent->y > ((int)height() - (int)width())) &&
	   (buttonEvent->y < (int)height()))
	{
		if(buttonEvent->direction == Direction::down)
		{
			addValue(1);
		}

		return;
	}

	if(buttonEvent->direction == Direction::down)
	{
		yOffset = buttonEvent->y;
		valueOffset = value();
	}

	dragging = (buttonEvent->direction == Direction::down);
}

} // GUI::

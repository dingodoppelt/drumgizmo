/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            progressbar.cc
 *
 *  Fri Mar 22 22:07:57 CET 2013
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
#include "progressbar.h"

namespace GUI
{

ProgressBar::ProgressBar(Widget *parent)
	: Widget(parent)
{
}

ProgressBar::~ProgressBar()
{
}

void ProgressBar::setState(ProgressBarState state)
{
	if(this->state != state)
	{
		this->state = state;
		redraw();
	}
}

void ProgressBar::setTotal(std::size_t total)
{
	if(this->total != total)
	{
		this->total = total;
		redraw();
	}
}

void ProgressBar::setValue(std::size_t value)
{
	if(this->value != value)
	{
		this->value = value;
		redraw();
	}
}

void ProgressBar::repaintEvent(RepaintEvent* repaintEvent)
{
	Painter p(*this);

	float progress = 0.0f;
	if(total != 0)
	{
		progress = (float)value / (float)total;
	}

	int brd = 4;
	int val = (width() - (2 * brd)) * progress;

	bar_bg.setSize(width(), height());
	p.drawImage(0, 0, bar_bg);

	switch(state)
	{
	case ProgressBarState::Red:
		bar_red.setSize(val, height());
		p.drawImage(brd, 0, bar_red);
		break;
	case ProgressBarState::Green:
		bar_green.setSize(val, height());
		p.drawImage(brd, 0, bar_green);
		break;
	case ProgressBarState::Blue:
		bar_blue.setSize(val, height());
		p.drawImage(brd, 0, bar_blue);
		break;
	case ProgressBarState::Off:
		return;
	}

}

} // GUI::

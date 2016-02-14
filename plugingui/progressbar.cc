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
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "progressbar.h"

namespace GUI {

ProgressBar::ProgressBar(Widget *parent)
	: Widget(parent)
{
	bar_bg.left    = new Image(":progress_back_l.png");
	bar_bg.right   = new Image(":progress_back_r.png");
	bar_bg.center  = new Image(":progress_back_c.png");

	bar_blue.left    = new Image(":progress_front_blue_l.png");
	bar_blue.right   = new Image(":progress_front_blue_r.png");
	bar_blue.center  = new Image(":progress_front_blue_c.png");

	bar_red.left    = new Image(":progress_front_red_l.png");
	bar_red.right   = new Image(":progress_front_red_r.png");
	bar_red.center  = new Image(":progress_front_red_c.png");

	bar_green.left    = new Image(":progress_front_green_l.png");
	bar_green.right   = new Image(":progress_front_green_r.png");
	bar_green.center  = new Image(":progress_front_green_c.png");

	state = ProgressBarState::Blue;
	_progress = .5;
}

ProgressBar::~ProgressBar()
{
	delete bar_bg.left;
	delete bar_bg.right;
	delete bar_bg.center;

	delete bar_blue.left;
	delete bar_blue.right;
	delete bar_blue.center;

	delete bar_red.left;
	delete bar_red.right;
	delete bar_red.center;

	delete bar_green.left;
	delete bar_green.right;
	delete bar_green.center;
}

void ProgressBar::setState(ProgressBarState state)
{
	if(this->state != state)
	{
		this->state = state;
		repaintEvent(nullptr);
	}
}

float ProgressBar::progress()
{
	return _progress;
}

void ProgressBar::setProgress(float progress)
{
	_progress = progress;
	repaintEvent(nullptr);
}

void ProgressBar::repaintEvent(RepaintEvent* repaintEvent)
{
	Painter p(*this);

	int max = width() * _progress;

	p.clear();

	int brd = 4;
	p.drawBar(0, 0, bar_bg, width(), height());

	Painter::Bar* bar = nullptr;
	switch(state) {
	case ProgressBarState::Red:
		bar = &bar_red;
		break;
	case ProgressBarState::Green:
		bar = &bar_green;
		break;
	case ProgressBarState::Blue:
		bar = &bar_blue;
		break;
	case ProgressBarState::Off:
		bar = nullptr;
		break;
	}

	if(bar)
	{
		p.drawBar(brd, 0, *bar, max - 2 * brd, height());
	}
}

} // GUI::

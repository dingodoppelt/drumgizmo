/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            checkbox.cc
 *
 *  Sat Nov 26 15:07:44 CET 2011
 *  Copyright 2011 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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
#include "checkbox.h"

#include "painter.h"

#include <stdio.h>

namespace GUI {

CheckBox::CheckBox(Widget *parent)
	: Widget(parent)
	, bg_on(":switch_back_on.png")
	, bg_off(":switch_back_off.png")
	, knob(":switch_front.png")
	, state(false)
	, middle(false)
{
}

void CheckBox::buttonEvent(ButtonEvent *e)
{
	if(e->direction == -1 || e->doubleclick)
	{
		middle = false;
		internalSetChecked(!state);
	}
	else
	{
		middle = true;
	}

	repaintEvent(nullptr);
}

void CheckBox::setText(std::string text)
{
	_text = text;
	repaintEvent(nullptr);
}

void CheckBox::keyEvent(KeyEvent *e)
{
	if(e->keycode == KeyEvent::KEY_CHARACTER && e->text == " ")
	{
		if(e->direction == -1)
		{
			middle = false;
			internalSetChecked(!state);
		}
		else
		{
			middle = true;
		}

		repaintEvent(nullptr);
	}
}

void CheckBox::repaintEvent(RepaintEvent *e)
{
	Painter p(this);

	p.clear();

	if(state)
	{
		p.drawImage(0, (knob.height() - bg_on.height()) / 2, &bg_on);
		if(middle)
		{
			p.drawImage((bg_on.width() - knob.width()) / 2 + 1, 0, &knob);
		}
		else
		{
			p.drawImage(bg_on.width() - 40 + 2, 0, &knob);
		}
	}
	else
	{
		p.drawImage(0, (knob.height() - bg_off.height()) / 2, &bg_off);
		if(middle)
		{
			p.drawImage((bg_on.width() - knob.width()) / 2 + 1, 0, &knob);
		}
		else
		{
			p.drawImage(0, 0, &knob);
		}
	}
}

bool CheckBox::checked()
{
	return state;
}

void CheckBox::setChecked(bool c)
{
	internalSetChecked(c);
}

void CheckBox::internalSetChecked(bool checked)
{
	if(checked == state)
	{
		return;
	}

	state = checked;
	stateChangedNotifier(state);
	repaintEvent(nullptr);
}

} // GUI::

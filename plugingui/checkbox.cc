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
#include "checkbox.h"

#include "painter.h"

namespace GUI
{

CheckBox::CheckBox(Widget* parent)
    : Toggle(parent)
    , bg_on(getImageCache(), ":resources/switch_back_on.png")
    , bg_off(getImageCache(), ":resources/switch_back_off.png")
    , knob(getImageCache(), ":resources/switch_front.png")
{
}

void CheckBox::repaintEvent(RepaintEvent* repaintEvent)
{
	Painter p(*this);

	p.drawImage(
	    0, (knob.height() - bg_on.height()) / 2, state ? bg_on : bg_off);

	if(clicked)
	{
		p.drawImage((bg_on.width() - knob.width()) / 2 + 1, 0, knob);
		return;
	}

	if(state)
	{
		p.drawImage(bg_on.width() - 40 + 2, 0, knob);
	}
	else
	{
		p.drawImage(0, 0, knob);
	}
}

} // GUI::

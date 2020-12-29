/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            verticalline.cc
 *
 *  Sat Apr  6 12:59:44 CEST 2013
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
#include "verticalline.h"

#include "painter.h"

namespace GUI {

VerticalLine::VerticalLine(Widget *parent)
	: Widget(parent)
	, vline(":resources/vertline.png")
{
}

void VerticalLine::repaintEvent(RepaintEvent* repaintEvent)
{
	if(height() < 2)
	{
		return;
	}

	Painter p(*this);
	p.drawImageStretched(0, (height() - vline.height()) / 2,
	                     vline, width(), vline.height());
}

} // GUI::

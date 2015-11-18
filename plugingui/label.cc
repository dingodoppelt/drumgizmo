/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            label.cc
 *
 *  Sun Oct  9 13:02:18 CEST 2011
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
#include "label.h"

#include "painter.h"
#include "guievent.h"

namespace GUI {

Label::Label(GUI::Widget *parent)
	: Widget(parent)
{
}

void Label::setText(std::string text)
{
	_text = text;
	repaintEvent(nullptr);
}

void Label::repaintEvent(GUI::RepaintEvent* repaintEvent)
{
	Painter p(*this);

	p.clear();

	p.setColour(Colour(1));

	Font font(":fontemboss.png");
	p.drawText(10, (height() + font.textHeight()) / 2, font, _text, true);
}

} // GUI::

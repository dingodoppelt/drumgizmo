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
#include "label.h"

#include "painter.h"
#include "guievent.h"

namespace GUI {

Label::Label(Widget *parent)
	: Widget(parent)
{
}

void Label::setText(const std::string& text)
{
	_text = text;
	repaintEvent(nullptr);
}

void Label::setAlignment(TextAlignment alignment)
{
	this->alignment = alignment;
}

void Label::resizeToText()
{
	resize(font.textWidth(_text) + border, font.textHeight());
}

void Label::repaintEvent(RepaintEvent* repaintEvent)
{
	Painter p(*this);

	p.clear();

	p.setColour(Colour(1));

	int offset = 0;
	switch(alignment) {
	case TextAlignment::left:
		offset = border;
		break;
	case TextAlignment::center:
		offset = (width() - font.textWidth(_text)) / 2;
		break;
	case TextAlignment::right:
		offset = width() - font.textWidth(_text) - border;
		break;
	}

	p.drawText(offset, (height() + font.textHeight()) / 2, font, _text, true);
}

} // GUI::

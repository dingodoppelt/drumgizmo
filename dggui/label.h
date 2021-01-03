/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            label.h
 *
 *  Sun Oct  9 13:02:17 CEST 2011
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
#pragma once

#include "widget.h"

#include "font.h"

#include <string>
#include <memory>

namespace dggui
{

enum class TextAlignment {
	left,
	center,
	right,
};

class Label : public Widget {
public:
	Label(Widget *parent);
	virtual ~Label() = default;

	void setText(const std::string& text);
	void setAlignment(TextAlignment alignment);
	void setColour(Colour colour);
	void resetColour();
	void resizeToText();

protected:
	// From Widget:
	virtual void repaintEvent(RepaintEvent* repaintEvent) override;

private:
	std::string _text;
	Font font{":resources/fontemboss.png"};
	TextAlignment alignment{TextAlignment::left};
	int border{0};

	// optional colour
	std::unique_ptr<Colour> colour;
};

} // dggui::

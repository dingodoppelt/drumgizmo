/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            button.h
 *
 *  Sun Oct  9 13:01:56 CEST 2011
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

#include "button_base.h"
#include "font.h"
#include "texturedbox.h"

namespace GUI {

class Button
	: public ButtonBase {
public:
	Button(Widget* parent);
	virtual ~Button();

protected:
	// From Widget:
	virtual void repaintEvent(RepaintEvent* e) override;

private:
	TexturedBox box_up{getImageCache(), ":pushbutton.png",
			0, 0, // atlas offset (x, y)
			7, 1, 7, // dx1, dx2, dx3
			6, 12, 9}; // dy1, dy2, dy3

	TexturedBox box_down{getImageCache(), ":pushbutton.png",
			15, 0, // atlas offset (x, y)
			7, 1, 7, // dx1, dx2, dx3
			6, 12, 9}; // dy1, dy2, dy3

	TexturedBox box_grey{getImageCache(), ":pushbutton.png",
			30, 0, // atlas offset (x, y)
			7, 1, 7, // dx1, dx2, dx3
			6, 12, 9}; // dy1, dy2, dy3

	Font font{":fontemboss.png"};
};

} // GUI::

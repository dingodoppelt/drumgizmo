/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            slider.h
 *
 *  Sat Nov 26 18:10:22 CET 2011
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

#include "texture.h"
#include "texturedbox.h"
#include "widget.h"

namespace GUI
{

class Slider
	: public Widget
{
public:
	Slider(Widget* parent);
	virtual ~Slider() = default;

	// From Widget:
	bool catchMouse() override
	{
		return true;
	}
	bool isFocusable() override
	{
		return true;
	}

	void setValue(float new_value);
	float value() const;

	Notifier<> clickNotifier;

protected:
	virtual void repaintEvent(RepaintEvent* repaintEvent) override;
	virtual void buttonEvent(ButtonEvent* buttonEvent) override;
	virtual void mouseMoveEvent(MouseMoveEvent* mouseMoveEvent) override;

private:
	enum class State
	{
		up,
		down
	};

	float current_value;
	float maximum;
	float minimum;

	State state;

	TexturedBox bar{
	    getImageCache(), ":slider.png", 0, 0, // atlas offset (x, y)
	    7, 1, 7,                              // dx1, dx2, dx3
	    7, 1, 7                               // dy1, dy2, dy3
	};
	TexturedBox inner_bar_green{
	    getImageCache(), ":slider.png", 30, 0, // atlas offset (x, y)
	    2, 1, 2,                               // dx1, dx2, dx3
	    2, 1, 2                                // dy1, dy2, dy3
	};
	// TexturedBox inner_bar_red{getImageCache(), ":slider.png",
	//     30, 5, // atlas offset (x, y)
	//     2, 1, 2, // dx1, dx2, dx3
	//     2, 1, 2 // dy1, dy2, dy3
	// };
	// TexturedBox inner_bar_blue{getImageCache(), ":slider.png",
	//     30, 10, // atlas offset (x, y)
	//     2, 1, 2, // dx1, dx2, dx3
	//     2, 1, 2 // dy1, dy2, dy3
	// };
	Texture button{
	    getImageCache(), ":slider.png", 15, 0, // atlas offset (x, y)
	    15, 15                                 // width, height
	};

	std::size_t bar_boundary{5};
	std::size_t button_offset{7};

	std::size_t getControlWidth() const;
	void recomputeCurrentValue(float x);
};

} // GUI::

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

#include "widget.h"

namespace GUI {

class Slider : public Widget {
public:
	Slider(Widget* parent);

	// From Widget:
	bool catchMouse() override { return true; }

	void setValue(float value);
	float value();

	Notifier<> clickNotifier;

protected:
	virtual void repaintEvent(RepaintEvent* repaintEvent) override;
	virtual void buttonEvent(ButtonEvent* buttonEvent) override;
	virtual void mouseMoveEvent(MouseMoveEvent* mouseMoveEvent) override;

private:
	enum class State {
		up,
		down
	};

	float currentValue;
	float maximum;
	float minimum;

	State state;
};

} // GUI::

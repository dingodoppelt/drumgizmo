/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            knob.h
 *
 *  Thu Feb 28 07:37:27 CET 2013
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
#pragma once

#include <notifier.h>

#include "widget.h"
#include "texture.h"
#include "font.h"

namespace GUI {

class Knob : public Widget {
public:
	Knob(Widget *parent);
	virtual ~Knob() = default;

	// From Widget:
	bool catchMouse() override { return true; }
	bool isFocusable() override { return true; }

	void setValue(float value);
	void setDefaultValue(float value);
	void setRange(float minimum, float maximum);
	float value();
	void showValue(bool show_value);

	Notifier<float> valueChangedNotifier; // (float newValue)

protected:
	virtual void clicked() {}

	// From Widget:
	virtual void repaintEvent(RepaintEvent* repaintEvent) override;
	virtual void buttonEvent(ButtonEvent* buttonEvent) override;
	virtual void mouseMoveEvent(MouseMoveEvent* mouseMoveEvent) override;
	virtual void scrollEvent(ScrollEvent* scrollEvent) override;
	virtual void keyEvent(KeyEvent* keyEvent) override;

private:
	//! Sets the internal value and sends out the changed notification.
	void internalSetValue(float value);

	typedef enum {
		up,
		down
	} state_t;

	state_t state;

	float current_value;
	float default_value = 0.0;
	float maximum;
	float minimum;

	bool show_value{true};

	Texture img_knob;

	int mouse_offset_x;
	Font font;
};

} // GUI::

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            checkbox.h
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
#pragma once

#include <notifier.h>

#include "widget.h"
#include "texture.h"

namespace GUI {

class CheckBox : public Widget {
public:
	CheckBox(Widget *parent);

	void setText(std::string text);

	// From Widget:
	bool isFocusable() override { return true; }
	bool catchMouse() override { return true; }

	bool checked();
	void setChecked(bool checked);

	Notifier<bool> stateChangedNotifier;

protected:
	// From Widget:
	virtual void repaintEvent(RepaintEvent* repaintEvent) override;
	virtual void buttonEvent(ButtonEvent* buttonEvent) override;
	virtual void keyEvent(KeyEvent* keyEvent) override;
	virtual void mouseLeaveEvent() override;
	virtual void mouseEnterEvent() override;

private:
	void internalSetChecked(bool checked);

	Texture bg_on;
	Texture bg_off;
	Texture knob;

	bool state{false};
	bool middle{false};
	bool buttonDown{false};
	bool inCheckbox{false};

	std::string _text;
};

} // GUI::

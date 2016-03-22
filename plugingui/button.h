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

#include <string>

#include "widget.h"
#include "painter.h"
#include "notifier.h"
#include "font.h"

namespace GUI {

class Button : public Widget {
public:
	Button(Widget *parent);
	~Button();

	// From Widget:
	bool isFocusable() override { return true; }
	bool catchMouse() override { return true; }

	void setText(const std::string& text);

	Notifier<> clickNotifier;

protected:
	virtual void clicked() {}

	// From Widget:
	virtual void repaintEvent(RepaintEvent* e) override;
	virtual void buttonEvent(ButtonEvent* e) override;
	virtual void mouseLeaveEvent() override;
	virtual void mouseEnterEvent() override;

private:
	bool in_button{false};

	Painter::Box box_up;
	Painter::Box box_down;

	typedef enum {
		up,
		down
	} state_t;

	std::string text;

	Font font{":fontemboss.png"};

	state_t draw_state{up};
	state_t button_state{up};
};

} // GUI::

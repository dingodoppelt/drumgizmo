/* -*- Mode: c++ -*- */
/***************************************************************************
 *            button_base.h
 *
 *  Sat Apr 15 21:45:30 CEST 2017
 *  Copyright 2017 André Nusser
 *  andre.nusser@googlemail.com
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

#include <notifier.h>

#include "widget.h"

namespace dggui
{

class ButtonBase
	: public Widget
{
public:
	ButtonBase(Widget* parent);
	virtual ~ButtonBase();

	// From Widget:
	bool isFocusable() override { return true; }
	bool catchMouse() override { return true; }

	void setText(const std::string& text);

	void setEnabled(bool enabled);
	bool isEnabled() const;

	Notifier<> clickNotifier;

protected:
	virtual void clicked() {}

	// From Widget:
	virtual void repaintEvent(RepaintEvent* e) override {};
	virtual void buttonEvent(ButtonEvent* e) override;
	virtual void mouseLeaveEvent() override;
	virtual void mouseEnterEvent() override;

	bool enabled{true};
	bool in_button{false};

	enum class State {
		Up,
		Down
	};

	std::string text;

	State draw_state{State::Up};
	State button_state{State::Up};
};

} // dggui::

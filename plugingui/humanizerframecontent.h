/* -*- Mode: c++ -*- */
/***************************************************************************
 *            humanizerframecontent.h
 *
 *  Fri Mar 24 21:49:58 CET 2017
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

#include "knob.h"
#include "label.h"
#include "widget.h"

#include <iomanip>
#include <sstream>

namespace GUI
{

// TODO move this to an own file?
class LabeledControl : public Widget
{
public:
	LabeledControl(Widget* parent, const std::string& name) : Widget(parent)
	{
		layout.setResizeChildren(false);
		layout.setHAlignment(HAlignment::center);
		layout.setSpacing(2);

		caption.setText(name);
		caption.resize(100, 20);
		caption.setAlignment(TextAlignment::center);
		layout.addItem(&caption);
	}

	void setControl(Knob* control)
	{
		layout.addItem(control);

		CONNECT(control, valueChangedNotifier, this, &LabeledControl::setValue);
		setValue(control->value());
		value.resize(100, 20);
		value.setAlignment(TextAlignment::center);
		layout.addItem(&value);
	}

private:
	VBoxLayout layout{this};
	Label caption{this};
	Label value{this};

	void setValue(float new_value)
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(2) << new_value;
		value.setText(stream.str());
	}
};

class HumanizerframeContent : public Widget
{
public:
	HumanizerframeContent(Widget* parent);

private:
	HBoxLayout layout{this};

	LabeledControl attack{this, "Attack"};
	LabeledControl falloff{this, "Release"};

	Knob attackKnob{&attack};
	Knob falloffKnob{&falloff};
};

} // GUI::

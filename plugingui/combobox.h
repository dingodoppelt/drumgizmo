/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            combobox.h
 *
 *  Sun Mar 10 19:04:50 CET 2013
 *  Copyright 2013 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#pragma once

#include <string.h>
#include <vector>

#include "widget.h"
#include "font.h"
#include "listboxthin.h"
#include "painter.h"

namespace GUI {

class ComboBox : public Widget {
public:
	ComboBox(Widget* parent);
	~ComboBox();

	void addItem(std::string name, std::string value);

	void clear();
	bool selectItem(int index);
	std::string selectedName();
	std::string selectedValue();

	// From Widget:
	bool isFocusable() override { return true; }
	virtual void repaintEvent(RepaintEvent* repaintEvent) override;
	virtual void buttonEvent(ButtonEvent* buttonEvent) override;
	virtual void scrollEvent(ScrollEvent* scrollEvent) override;
	virtual void keyEvent(KeyEvent* keyEvent) override;

	Notifier<std::string, std::string> valueChangedNotifier;

private:
	Painter::Box box;

	void listboxSelectHandler();

	Font font;
	ListBoxThin listbox;
};

} // GUI::

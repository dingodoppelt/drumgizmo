/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            listboxbasic.h
 *
 *  Thu Apr  4 20:28:10 CEST 2013
 *  Copyright 2013 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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
#include "painter.h"
#include "scrollbar.h"
#include "notifier.h"

namespace GUI {

class ListBoxBasic : public Widget {
public:
	class Item {
	public:
		std::string name;
		std::string value;
	};

	ListBoxBasic(Widget *parent);
	~ListBoxBasic();

	void addItem(const std::string& name, const std::string& value);
	void addItems(const std::vector<Item>& items);

	void clear();
	bool selectItem(int index);
	std::string selectedName();
	std::string selectedValue();

	void clearSelectedValue();

	Notifier<> selectionNotifier;
	Notifier<> clickNotifier;
	Notifier<> valueChangedNotifier;

	// From Widget:
	virtual void resize(int w, int h) override;

protected:
	void onScrollBarValueChange(int value);

	// From Widget:
	bool isFocusable() override { return true; }
	virtual void repaintEvent(RepaintEvent *e) override;
	virtual void buttonEvent(ButtonEvent *e) override;
	virtual void keyEvent(KeyEvent *e) override;
	virtual void scrollEvent(ScrollEvent *e) override;

	ScrollBar scroll;

	Image bg_img;

	void setSelection(int index);

	std::vector<Item> items;

	int selected;
	int marked;
	Font font;
	int padding;
	int btn_size;
};

} // GUI::

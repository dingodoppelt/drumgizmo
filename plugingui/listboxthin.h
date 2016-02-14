/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            listboxthin.h
 *
 *  Sun Apr  7 19:39:35 CEST 2013
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
#include "painter.h"
#include "listboxbasic.h"
#include "notifier.h"

namespace GUI {

class ListBoxThin : public Widget {
public:
	ListBoxThin(Widget *parent);
	~ListBoxThin();

	void addItem(std::string name, std::string value);
	void addItems(std::vector<ListBoxBasic::Item> &items);

	void clear();
	bool selectItem(int index);
	std::string selectedName();
	std::string selectedValue();

	// From Widget:
	virtual void repaintEvent(GUI::RepaintEvent* repaintEvent) override;
	virtual void resize(int w, int h) override;

	// Forwarded notifier from ListBoxBasic::basic
	Notifier<>& selectionNotifier;
	Notifier<>& clickNotifier;
	Notifier<>& valueChangedNotifier;

private:
	ListBoxBasic basic;

	Painter::Box box;
};

} // GUI::

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            scrollbar.h
 *
 *  Sun Apr 14 12:54:58 CEST 2013
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

#include "widget.h"
#include "image.h"
#include "notifier.h"

namespace GUI {

class ScrollBar : public Widget {
	friend class ListBoxBasic;
public:
	ScrollBar(Widget *parent);

	void setRange(int range);
	int range();

	void setMaximum(int max);
	int maximum();

	void addValue(int delta);
	void setValue(int value);
	int value();

	Notifier<int> valueChangeNotifier; // (int value)

protected:
	// From Widget:
	bool catchMouse() override { return true; }
	void scrollEvent(ScrollEvent* scrollEvent) override;
	void repaintEvent(RepaintEvent* repaintEvent) override;
	void buttonEvent(ButtonEvent* buttonEvent) override;
	void mouseMoveEvent(MouseMoveEvent* mouseMoveEvent) override;

private:
	int maxValue;
	int currentValue;
	int rangeValue;

	int yOffset;
	int valueOffset;
	bool dragging;

	Image bg_img;
};

} // GUI::

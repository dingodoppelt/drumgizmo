/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            tabbutton.h
 *
 *  Thu Nov 24 18:52:26 CET 2016
 *  Copyright 2016 Bent Bisballe Nyeng
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

#include "button_base.h"
#include "font.h"
#include "texturedbox.h"

namespace GUI
{

class ScrollEvent;

using TabID = int;

class TabButton
	: public ButtonBase
{
public:
	TabButton(Widget* parent, Widget* tab_widget);
	virtual ~TabButton();

	Widget* getTabWidget();
	std::size_t getMinimalWidth() const;
	std::size_t getMinimalHeight() const;
	void setActive(bool active);

	TabID getID() const;

	Notifier<Widget*> switchTabNotifier;
	Notifier<float> scrollNotifier; // float delta

protected:
	// From Widget:
	virtual void repaintEvent(RepaintEvent* e) override;
	virtual void scrollEvent(ScrollEvent* scroll_event) override;

private:
	TabID tab_id;

	void clickHandler();

	Widget* tab_widget;
	bool active{false};

	TexturedBox tab_active{getImageCache(), ":resources/tab.png",
			0, 0, // atlas offset (x, y)
			5, 1, 5, // dx1, dx2, dx3
			5, 13, 1}; // dy1, dy2, dy3

	TexturedBox tab_passive{getImageCache(), ":resources/tab.png",
			11, 0, // atlas offset (x, y)
			5, 1, 5, // dx1, dx2, dx3
			5, 13, 1}; // dy1, dy2, dy3

	Font font{":resources/fontemboss.png"};
};

} // GUI::

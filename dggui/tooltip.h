/* -*- Mode: c++ -*- */
/***************************************************************************
 *            tooltip.h
 *
 *  Wed May  8 17:31:42 CEST 2019
 *  Copyright 2019 Bent Bisballe Nyeng
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

#include <string.h>
#include <vector>

#include <notifier.h>

#include "widget.h"
#include "painter.h"
#include "texturedbox.h"
#include "font.h"

namespace GUI
{

class Tooltip
	: public Widget
{
public:
	Tooltip(Widget *parent);
	virtual ~Tooltip();

	void setText(const std::string& text);

	// From Widget:
	virtual void repaintEvent(RepaintEvent* repaint_event) override;
	virtual void resize(std::size_t height, std::size_t width) override;
	virtual void mouseLeaveEvent() override;
	virtual void show() override;
	virtual void buttonEvent(ButtonEvent* buttonEvent) override;

private:
	void preprocessText();

	TexturedBox box{getImageCache(), ":resources/thinlistbox.png",
			0, 0, // atlas offset (x, y)
			1, 1, 1, // dx1, dx2, dx3
			1, 1, 1}; // dy1, dy2, dy3
	Font font;

	static constexpr int x_border{10};
	static constexpr int y_border{8};

	bool needs_preprocessing{false};
	std::string text;
	std::vector<std::string> preprocessed_text;
	std::size_t max_text_width{0};
	std::size_t total_text_height{0};
	Widget* activating_widget;
};

} // GUI::

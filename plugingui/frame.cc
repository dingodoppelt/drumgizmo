/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            frame.cc
 *
 *  Tue Feb  7 21:07:56 CET 2017
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
#include "frame.h"

#include "painter.h"

namespace GUI
{

FrameWidget::FrameWidget(Widget* parent, bool has_switch)
    : Widget(parent), is_switched_on(!has_switch), bar_height(24)
{
	if(has_switch)
	{
		// We only have to set this once as nothing happens on a resize
		power_button.move(4, 4);
		power_button.resize(16, 16);

		CONNECT(&power_button, stateChangedNotifier, this,
		    &FrameWidget::powerButtonClicked);
	}
	power_button.setVisible(has_switch);

	CONNECT(this, sizeChangeNotifier, this, &FrameWidget::sizeChanged);
}

void FrameWidget::repaintEvent(RepaintEvent* repaintEvent)
{
	Painter p(*this);
	p.clear();

	int center_x = width() / 2;
	auto title_buf = title.c_str();

	// draw the dark grey box
	p.setColour(grey_box_colour);
	p.drawFilledRectangle(1, 1, width() - 2, bar_height);

	// frame
	p.setColour(frame_colour_top);
	p.drawLine(0, 0, width() - 1, 0);
	p.setColour(frame_colour_bottom);
	p.drawLine(0, height() - 1, width() - 1, height() - 1);
	p.setColour(frame_colour_side);
	p.drawLine(0, 0, 0, height() - 1);
	p.drawLine(width() - 1, 0, width() - 1, height() - 1);

	// background
	p.setColour(background_colour);
	p.drawFilledRectangle(1, bar_height, width() - 2, height() - 2);

	// draw the label
	p.setColour(label_colour);
	p.drawText(center_x - label_width, bar_height - 4, font, title_buf);
}

void FrameWidget::powerButtonClicked(bool clicked)
{
	is_switched_on = !is_switched_on;

	if(content)
	{
		content->setVisible(true);
	}
}

void FrameWidget::setTitle(std::string const& title)
{
	this->title = title;
	label_width = font.textWidth(title.c_str()) / 2 + 1;
}

void FrameWidget::setContent(Widget* content)
{
	this->content = content;
	content->reparent(this);
	content->setVisible(true);
}

void FrameWidget::sizeChanged(int width, int height)
{
	if(content)
	{
		content_start_x = content_margin;
		content_start_y = bar_height + content_margin;
		content_width = width - 2 * content_margin;
		content_height = height - (bar_height + 2 * content_margin);

		content->move(content_start_x, content_start_y);
		content->resize(content_width, content_height);
	}
}

} // GUI::

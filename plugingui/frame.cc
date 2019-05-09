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

FrameWidget::FrameWidget(Widget* parent, bool has_switch, bool has_help_text)
	: Widget(parent)
	, is_switched_on(!has_switch)
	, bar_height(24)
{
	if(has_switch)
	{
		// We only have to set this once as nothing happens on a resize
		power_button.move(4, 4);
		power_button.resize(16, 16);

		power_button.setChecked(is_switched_on);
		CONNECT(&power_button, stateChangedNotifier, this,
		        &FrameWidget::powerButtonStateChanged);
	}
	power_button.setVisible(has_switch);

	if(has_help_text)
	{
		// We only have to set this once as nothing happens on a resize
		help_button.resize(16, 16);
		help_button.move(width() - 4 - 16, 4);
		help_button.setText("?");
	}
	help_button.setVisible(has_help_text);

	CONNECT(this, sizeChangeNotifier, this, &FrameWidget::sizeChanged);
}

void FrameWidget::repaintEvent(RepaintEvent* repaintEvent)
{
	Painter p(*this);

	int center_x = width() / 2;
	auto title_buf = title.c_str();

	// draw the dark grey box
	p.setColour(enabled ? grey_box_colour : grey_box_colour_disabled);
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
	p.setColour(enabled ? label_colour : label_colour_disabled);
	p.drawText(center_x - label_width, bar_height - 4, font, title_buf);
	power_button.setEnabled(enabled);
}

void FrameWidget::powerButtonStateChanged(bool new_state)
{
	is_switched_on = new_state;
	onSwitchChangeNotifier(is_switched_on);
}

void FrameWidget::setTitle(std::string const& title)
{
	this->title = title;
	label_width = font.textWidth(title.c_str()) / 2 + 1;
}

void FrameWidget::setHelpText(const std::string& help_text)
{
	help_button.setHelpText(help_text);
}

void FrameWidget::setContent(Widget* content)
{
	this->content = content;
	content->reparent(this);
}

void FrameWidget::setOnSwitch(bool on)
{
	is_switched_on = on;
	power_button.setChecked(is_switched_on);
}

void FrameWidget::setEnabled(bool enabled)
{
	this->enabled = enabled;
	onEnabledChanged(enabled);

	redraw();
}

void FrameWidget::sizeChanged(int width, int height)
{
	if(content)
	{
		content_start_x = content_margin;
		content_start_y = bar_height + content_margin;
		content_width = std::max((int)width - 2 * content_margin, 0);
		content_height = std::max((int)height - (bar_height + 2 * content_margin), 0);

		content->move(content_start_x, content_start_y);
		content->resize(content_width, content_height);
	}

	help_button.move(width - 4 - 16, help_button.y());
}

} // GUI::

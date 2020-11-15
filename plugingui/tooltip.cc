/* -*- Mode: c++ -*- */
/***************************************************************************
 *            tooltip.cc
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
#include "tooltip.h"

#include "painter.h"
#include "font.h"
#include "window.h"
#include <iostream>

namespace GUI
{

Tooltip::Tooltip(Widget *parent)
	: Widget(parent->window())
	, activating_widget(parent)
{
	resize(32, 32);
}

Tooltip::~Tooltip()
{
}

void Tooltip::setText(const std::string& text)
{
	this->text = text;
	needs_preprocessing = true;
	redraw();
}

void Tooltip::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);
}

void Tooltip::repaintEvent(RepaintEvent* repaintEvent)
{
	if(needs_preprocessing)
	{
		preprocessText();
	}

	Painter p(*this);

	if((width() == 0) || (height() == 0))
	{
		return;
	}

	box.setSize(width(), height());
	p.drawImage(0, 0, box);
	p.setColour(Colour(183.0f/255.0f, 219.0f/255.0f, 255.0f/255.0f, 1.0f));

	int ypos = font.textHeight() + y_border;

	for(std::size_t i = 0; i < preprocessed_text.size(); ++i)
	{
		if(i * font.textHeight() >= (height() - y_border - font.textHeight()))
		{
			break;
		}

		auto const& line = preprocessed_text[i];
		p.drawText(x_border, ypos, font, line);
		ypos += font.textHeight();
	}
}

void Tooltip::preprocessText()
{
	std::vector<std::string> lines;

	preprocessed_text.clear();
	std::string text = this->text;

	// Handle tab characters
	for(std::size_t i = 0; i < text.length(); ++i)
	{
		char ch = text.at(i);
		if(ch == '\t')
		{
			text.erase(i, 1);
			text.insert(i, 4, ' ');
		}
	}

	// Handle "\r"
	for(std::size_t i = 0; i < text.length(); ++i)
	{
		char ch = text.at(i);
		if(ch == '\r')
		{
			text.erase(i, 1);
		}
	}

	// Handle new line characters
	std::size_t pos = 0;
	do
	{
		pos = text.find("\n");
		lines.push_back(text.substr(0, pos));
		text = text.substr(pos + 1);
	} while(pos != std::string::npos);

	max_text_width = 0;
	total_text_height = 0;
	for(auto const& line: lines)
	{
		std::string valid;
		std::string current;
		for(auto c: line)
		{
			current += c;
			if(c == ' ')
			{
				valid.append(current.substr(valid.size()));
			}
		}
		preprocessed_text.push_back(current);

		max_text_width = std::max(max_text_width, font.textWidth(line));
		total_text_height += font.textHeight(line);
	}
}

void Tooltip::mouseLeaveEvent()
{
	hide();
}

void Tooltip::show()
{
	if(needs_preprocessing)
	{
		preprocessText();
	}

	resize(max_text_width + 2*x_border, total_text_height + 2*y_border);

	int x = activating_widget->translateToWindowX();
	int y = activating_widget->translateToWindowY();

	if(x + width() > window()->width())
	{
		x -= width();
		x += activating_widget->width();
	}

	if(y + height() > window()->height())
	{
		y -= height();
		y += activating_widget->height();
	}

	// Make sure the tip is displayed inside the window
	x = std::max(x, 0);
	y = std::max(y, 0);

	move(x, y);
	Widget::show();

	// TODO: This should be handled differently
	// Hack to notify the window that the mouse is now inside the tooltip.
	window()->setMouseFocus(this);
}

void Tooltip::buttonEvent(ButtonEvent* buttonEvent)
{
	if(buttonEvent->direction == Direction::down)
	{
		hide();
	}
}

} // GUI::

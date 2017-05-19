/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            lineedit.cc
 *
 *  Tue Oct 21 11:25:26 CEST 2014
 *  Copyright 2014 Jonas Suhr Christensen
 *  jsc@umbraculum.org
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
#include "textedit.h"

#include "painter.h"

namespace GUI
{

TextEdit::TextEdit(Widget* parent) : Widget(parent), scroll(this)
{
	setReadOnly(true);

	scroll.move(width() - 2*x_border - 3, y_border - 1);
	scroll.resize(16, 100);
	CONNECT(&scroll, valueChangeNotifier, this, &TextEdit::scrolled);
}

TextEdit::~TextEdit()
{
}

void TextEdit::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);

	needs_preprocessing = true;
	scroll.move(width - 2*x_border - 3, y_border - 1);
	scroll.resize(scroll.width(), std::max((int)height - 2*(y_border - 1), 0));
}

void TextEdit::setReadOnly(bool readonly)
{
	this->readonly = readonly;
}

bool TextEdit::readOnly()
{
	return readonly;
}

void TextEdit::setText(const std::string& text)
{
	this->text = text;

	needs_preprocessing = true;
	redraw();
	textChangedNotifier();
}

std::string TextEdit::getText()
{
	return text;
}

void TextEdit::preprocessText()
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

	// Wrap long lines
	for(auto it = lines.begin(); it != lines.end(); ++it)
	{
		auto line = *it;

		for(std::size_t i = 0; i < line.length(); ++i)
		{
			auto linewidth = font.textWidth(line.substr(0, i));
			if(linewidth >= width() - 2*x_border - 10 - scroll.width())
			{
				preprocessed_text.push_back(line.substr(0, i));
				line = line.substr(i);
				i = 0;
			}
		}
		preprocessed_text.push_back(line);
	}
}

void TextEdit::repaintEvent(RepaintEvent* repaintEvent)
{
	if(needs_preprocessing) 
	{
		preprocessText();
	}

	Painter p(*this);

	// update values of scroll bar
	scroll.setRange(height() / font.textHeight());
	scroll.setMaximum(preprocessed_text.size());

	if((width() == 0) || (height() == 0))
	{
		return;
	}

	box.setSize(width(), height());
	p.drawImage(0, 0, box);
	p.setColour(Colour(183.0 / 255.0, 219.0 / 255.0, 255.0 / 255.0, 1));

	int ypos = font.textHeight() + y_border;

	auto scroll_value = scroll.value();
	for(std::size_t i = 0; i < preprocessed_text.size() - scroll_value; ++i)
	{
		if(i * font.textHeight() >= (height() - y_border - font.textHeight()))
		{
			break;
		}

		auto const& line = preprocessed_text[scroll_value + i];
		p.drawText(x_border, ypos, font, line);
		ypos += font.textHeight();
	}
}

void TextEdit::scrollEvent(ScrollEvent* scrollEvent)
{
	scroll.setValue(scroll.value() + scrollEvent->delta);
}

void TextEdit::scrolled(int value)
{
	(void)value;
	redraw();
}

} // GUI::

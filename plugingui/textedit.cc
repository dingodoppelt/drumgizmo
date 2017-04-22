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

#include "window.h"

#include <assert.h>
#include <hugin.hpp>
#include <list>
#include <stdio.h>

#define BORDER 10

namespace GUI
{

TextEdit::TextEdit(Widget* parent) : Widget(parent), scroll(this)
{
	setReadOnly(true);

	scroll.move(width() - 23, 1);
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
	scroll.resize(scroll.width(), height - 14);
	scroll.move(width - 23, 7);
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
	_text = text;

	needs_preprocessing = true;
	redraw();
	textChangedNotifier();
}

std::string TextEdit::text()
{
	return _text;
}

void TextEdit::preprocessText()
{
	preprocessedtext.clear();
	std::string text = _text;

	{ // Handle tab characters
		for(size_t i = 0; i < text.length(); ++i)
		{
			char ch = text.at(i);
			if(ch == '\t')
			{
				text.erase(i, 1);
				text.insert(i, 4, ' ');
			}
		}
	}

	{ // Handle "\r"
		for(size_t i = 0; i < text.length(); ++i)
		{
			char ch = text.at(i);
			if(ch == '\r')
			{
				text.erase(i, 1);
			}
		}
	}

	std::list<std::string> lines;
	{ // Handle new line characters
		size_t pos = 0;
		do
		{
			pos = text.find("\n");
			lines.push_back(text.substr(0, pos));
			text = text.substr(pos + 1);
		} while(pos != std::string::npos);
	}

	{ // Wrap long lines
		std::list<std::string>::iterator it;
		for(it = lines.begin(); it != lines.end(); ++it)
		{
			std::string line = *it;

			for(size_t i = 0; i < line.length(); ++i)
			{
				size_t linewidth = font.textWidth(line.substr(0, i));
				if(linewidth >= width() - BORDER - 20 - scroll.width())
				{
					preprocessedtext.push_back(line.substr(0, i));
					line = line.substr(i);
					i = 0;
				}
			}
			preprocessedtext.push_back(line);
		}
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
	scroll.setMaximum(preprocessedtext.size());

	int w = width();
	int h = height();
	if((w == 0) || (h == 0))
	{
		return;
	}

	box.setSize(w, h);
	p.drawImage(0, 0, box);

	p.setColour(Colour(183.0 / 255.0, 219.0 / 255.0, 255.0 / 255.0, 1));

	int skip = scroll.value();

	int ypos = font.textHeight() + 5 + 1 + 1 + 1;
	std::list<std::string>::iterator it;
	it = preprocessedtext.begin();

	int c = 0;
	for(; c < skip; c++)
	{
		++it;
	}

	c = 0;
	for(; it != preprocessedtext.end(); it++)
	{
		if((c * font.textHeight()) >= (height() - 8 - font.textHeight()))
		{
			break;
		}

		std::string line = *it;
		p.drawText(BORDER - 4 + 3, ypos, font, line);
		ypos += font.textHeight();
		++c;
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

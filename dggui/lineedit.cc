/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            lineedit.cc
 *
 *  Sun Oct  9 13:01:52 CEST 2011
 *  Copyright 2011 Bent Bisballe Nyeng
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
#include "lineedit.h"

#include <stdio.h>
#include <hugin.hpp>

#define BORDER 10

namespace dggui
{

LineEdit::LineEdit(Widget *parent)
	: Widget(parent)
{
	setReadOnly(false);
}

LineEdit::~LineEdit()
{
}

void LineEdit::setReadOnly(bool ro)
{
	readonly = ro;
}

bool LineEdit::readOnly()
{
	return readonly;
}

void LineEdit::setText(const std::string& text)
{
	_text = text;
	pos = text.size();

	visibleText = _text;
	offsetPos = 0;

	redraw();
	textChanged();
}

std::string LineEdit::getText()
{
	return _text;
}

void LineEdit::buttonEvent(ButtonEvent *buttonEvent)
{
	if(readOnly())
	{
		return;
	}

	// Ignore everything except left clicks.
	if(buttonEvent->button != MouseButton::left)
	{
		return;
	}

	if(buttonEvent->direction == Direction::down)
	{
		for(int i = 0; i < (int)visibleText.length(); ++i)
		{
			int textWidth = font.textWidth(visibleText.substr(0, i));
			if(buttonEvent->x < (textWidth + BORDER))
			{
				pos = i + offsetPos;
				break;
			}
		}
		redraw();
	}
}

void LineEdit::keyEvent(KeyEvent *keyEvent)
{
	if(readOnly())
	{
		return;
	}

	bool change = false;

	if(keyEvent->direction == Direction::down)
	{
		switch(keyEvent->keycode) {
		case Key::left:
			if(pos == 0)
			{
				return;
			}

			pos--;

			if(offsetPos >= pos)
			{
				walkstate = WalkLeft;
			}
			break;

		case Key::right:
			if(pos == _text.length())
			{
				return;
			}

			pos++;

			if((pos < _text.length()) && ((offsetPos + visibleText.length()) <= pos))
			{
				walkstate = WalkRight;
			}
			break;

		case Key::home:
			pos = 0;
			visibleText = _text;
			offsetPos = 0;
			break;

		case Key::end:
			pos = _text.length();
			visibleText = _text;
			offsetPos = 0;
			break;

		case Key::deleteKey:
			if(pos < _text.length())
			{
				std::string t = _text.substr(0, pos);
				t += _text.substr(pos + 1, std::string::npos);
				_text = t;
				change = true;
			}
			break;

		case Key::backspace:
			if(pos > 0)
			{
				std::string t = _text.substr(0, pos - 1);
				t += _text.substr(pos, std::string::npos);
				_text = t;
				pos--;
				change = true;
			}
			break;

		case Key::character:
			{
				std::string pre = _text.substr(0, pos);
				std::string post = _text.substr(pos, std::string::npos);
				_text = pre + keyEvent->text + post;
				change = true;
				pos++;
			}
			break;

		case Key::enter:
			enterPressedNotifier();
	    break;

		default:
			break;
		}

		redraw();
	}

	if(change)
	{
		textChanged();
	}
}

void LineEdit::repaintEvent(RepaintEvent *repaintEvent)
{
	Painter p(*this);

	int w = width();
	int h = height();
	if((w == 0) || (h == 0))
	{
		return;
	}

	box.setSize(w, h);
	p.drawImage(0, 0, box);

	p.setColour(Colour(183.0f/255.0f, 219.0f/255.0f, 255.0f/255.0f, 1.0f));

	switch(walkstate) {
	case WalkLeft:
		visibleText = _text.substr(pos, std::string::npos);
		offsetPos = pos;
		break;

	case WalkRight:
		{
			int delta = (offsetPos < _text.length()) ? 1 : 0;
			visibleText = _text.substr(offsetPos + delta);
			offsetPos = offsetPos + delta;
		}
		break;

	case Noop:
		visibleText = _text;
		offsetPos = 0;
		break;
	}

	while(true)
	{
		int textWidth = font.textWidth(visibleText);
		if(textWidth <= std::max(w - BORDER - 4 + 3, 0))
		{
			break;
		}

		switch(walkstate) {
		case WalkLeft:
			visibleText = visibleText.substr(0, visibleText.length() - 1);
			break;

		case WalkRight:
			visibleText = visibleText.substr(0, visibleText.length() - 1);
			break;

		case Noop:
			if(offsetPos < pos)
			{
				visibleText = visibleText.substr(1);
				offsetPos++;
			}
			else
			{
				visibleText = visibleText.substr(0, visibleText.length() - 1);
			}
			break;
		}
	}

	walkstate = Noop;

	p.drawText(BORDER - 4 + 3, height() / 2 + 5 + 1 + 1 + 1, font, visibleText);

	if(readOnly())
	{
		return;
	}

	if(hasKeyboardFocus())
	{
		size_t px = font.textWidth(visibleText.substr(0, pos - offsetPos));
		p.drawLine(px + BORDER - 1 - 4 + 3, 6,
		           px + BORDER - 1 - 4 + 3, height() - 7);
	}
}

} // dggui::

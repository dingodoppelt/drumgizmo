/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            lineedit.h
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
#pragma once

#include <string>

#include "widget.h"
#include "font.h"
#include "painter.h"
#include "texturedbox.h"

namespace dggui
{

class LineEdit
	: public Widget
{
public:
	LineEdit(Widget *parent);
	virtual ~LineEdit();

	bool isFocusable() override { return true; }

	std::string getText();
	void setText(const std::string& text);

	void setReadOnly(bool readonly);
	bool readOnly();

	Notifier<> enterPressedNotifier;

	//protected:
	virtual void keyEvent(KeyEvent *keyEvent) override;
	virtual void repaintEvent(RepaintEvent *repaintEvent) override;
	virtual void buttonEvent(ButtonEvent *buttonEvent) override;

protected:
	virtual void textChanged() {}

private:
	TexturedBox box{getImageCache(), ":resources/widget.png",
			0, 0, // atlas offset (x, y)
			7, 1, 7, // dx1, dx2, dx3
			7, 63, 7}; // dy1, dy2, dy3

	Font font;

	std::string _text;
	size_t pos{0};
	std::string visibleText;
	size_t offsetPos{0};

	enum state_t {
		Noop,
		WalkLeft,
		WalkRight,
	};
	state_t walkstate{Noop};

	bool readonly;
};

} // dggui::

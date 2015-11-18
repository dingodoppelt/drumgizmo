/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            widget.h
 *
 *  Sun Oct  9 13:01:44 CEST 2011
 *  Copyright 2011 Bent Bisballe Nyeng
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

#include "guievent.h"
#include "pixelbuffer.h"
#include "notifier.h"
#include "layout.h"

#include <vector>

namespace GUI {

class Window;

class Widget : public Listener, LayoutItem {
public:
	Widget(Widget *parent);
	virtual ~Widget();

	virtual void show();
	virtual void hide();

	// From LayoutItem
	virtual void resize(int width, int height) override;
	virtual void move(size_t x, size_t y) override;
	virtual size_t x() override;
	virtual size_t y() override;
	virtual size_t width() override;
	virtual size_t height() override;

	virtual size_t windowX();
	virtual size_t windowY();

	virtual bool isFocusable() { return false; }
	virtual bool catchMouse() { return false; }

	void addChild(Widget *widget);
	void removeChild(Widget *widget);

	virtual void repaintEvent(RepaintEvent* repaintEvent) {}
	virtual void mouseMoveEvent(MouseMoveEvent* mouseMoveEvent) {}
	virtual void buttonEvent(ButtonEvent* buttonEvent) {}
	virtual void scrollEvent(ScrollEvent* scrollEvent) {}
	virtual void keyEvent(KeyEvent* keyEvent) {}

	virtual void mouseLeaveEvent() {}
	virtual void mouseEnterEvent() {}

	Widget* find(size_t x, size_t y);

	virtual Window* window();

	void repaint_r(RepaintEvent* repaintEvent);

	PixelBufferAlpha pixbuf;
	std::vector<PixelBufferAlpha*> getPixelBuffers();

	bool hasKeyboardFocus();

	Widget *parent;

	bool visible();
	void setVisible(bool visible);

protected:
	std::vector<Widget*> children;
	Window *_window;
	size_t _x, _y, _width, _height;

private:
	bool _visible;
};

} // GUI::

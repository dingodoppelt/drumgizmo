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

#include "guievent.h"
#include "pixelbuffer.h"
#include "notifier.h"
#include "layout.h"
#include "canvas.h"

#include <vector>

namespace GUI
{

struct Point
{
	int x;
	int y;
};

struct Size
{
	std::size_t width;
	std::size_t height;
};

class ImageCache;
class Window;

class Widget
	: public Listener
	, public LayoutItem
	, public Canvas
{
	friend class Painter;
public:
	Widget(Widget* parent);
	virtual ~Widget();

	virtual void show();
	virtual void hide();
	void setVisible(bool visible);
	virtual bool visible() const;

	//! Mark widget dirty and shedule redraw on next window redraw.
	void redraw();

	// From LayoutItem
	virtual void resize(std::size_t width, std::size_t height) override;
	virtual void move(int x, int y) override;
	virtual int x() const override;
	virtual int y() const override;
	virtual std::size_t width() const override;
	virtual std::size_t height() const override;

	Point position() const;

	// From Canvas
	PixelBufferAlpha& getPixelBuffer() override;

	virtual bool isFocusable() { return false; }
	virtual bool catchMouse() { return false; }

	void addChild(Widget* widget);
	void removeChild(Widget* widget);
	void reparent(Widget* parent);

	virtual void repaintEvent(RepaintEvent* repaintEvent) {}
	virtual void mouseMoveEvent(MouseMoveEvent* mouseMoveEvent) {}
	virtual void buttonEvent(ButtonEvent* buttonEvent) {}
	virtual void scrollEvent(ScrollEvent* scrollEvent) {}
	virtual void keyEvent(KeyEvent* keyEvent) {}
	virtual void mouseLeaveEvent() {}
	virtual void mouseEnterEvent() {}

	virtual ImageCache& getImageCache();

	Widget* find(int x, int y);

	virtual Window* window();

	std::vector<PixelBufferAlpha*> getPixelBuffers();

	bool hasKeyboardFocus();

	Notifier<std::size_t, std::size_t> sizeChangeNotifier; // (width, height)
	Notifier<int, int> positionChangeNotifier; // (x, y)

	//! Translate x-coordinate from parent-space to window-space.
	virtual std::size_t translateToWindowX();

	//! Translate y-coordinate from parent-space to window-space.
	virtual std::size_t translateToWindowY();

protected:
	friend class EventHandler;
	PixelBufferAlpha pixbuf{0,0};

	std::vector<Widget*> children;

	Widget* parent = nullptr;
	Window* _window = nullptr;

	int _x{0};
	int _y{0};
	std::size_t _width{0};
	std::size_t _height{0};

	bool _visible{true};

	bool dirty{true};
};

} // GUI::

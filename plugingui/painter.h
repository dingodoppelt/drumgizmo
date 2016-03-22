/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            painter.h
 *
 *  Wed Oct 12 19:48:45 CEST 2011
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
#include "colour.h"
#include "pixelbuffer.h"
#include "font.h"
#include "image.h"

namespace GUI {

class Painter {
public:
	Painter(Widget& widget);
	~Painter();

	void flush();

	void setColour(const Colour& colour);

	void drawLine(int x1, int y1, int x2, int y2);
	void drawText(int x, int y, const Font& font, const std::string& text,
	              bool nocolour = false);
	void drawRectangle(int x1, int y1, int x2, int y2);
	void drawFilledRectangle(int x1, int y1, int x2, int y2);
	void drawPoint(int x, int y);
	void drawCircle(int x, int y, double r);
	void drawFilledCircle(int x, int y, int r);
	void drawImage(int x, int y, const Image& image);
	void drawImageStretched(int x, int y, const Image& image,
	                        int width, int height);

	typedef struct {
		Image* topLeft;
		Image* top;
		Image* topRight;
		Image* left;
		Image* right;
		Image* bottomLeft;
		Image* bottom;
		Image* bottomRight;
		Image* center;
	} Box;
	void drawBox(int x, int y, const Box& box, int width, int height);

	typedef struct {
		Image* left;
		Image* right;
		Image* center;
	} Bar;
	void drawBar(int x, int y, const Bar& bar, int width, int height);

	void clear();

private:
	Widget& widget;
	PixelBufferAlpha* pixbuf;
	Colour colour;
};

} // GUI::

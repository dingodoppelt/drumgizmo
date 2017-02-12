/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            paintertest.cc
 *
 *  Fri Nov 29 18:08:57 CET 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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
#include <cppunit/extensions/HelperMacros.h>

#include "../plugingui/canvas.h"
#include "../plugingui/painter.h"
#include "../plugingui/image.h"
#include "../plugingui/font.h"

class TestableCanvas
	: public GUI::Canvas
{
public:
	TestableCanvas(std::size_t width, std::size_t height)
		: pixbuf(width, height)
	{}

	GUI::PixelBufferAlpha& GetPixelBuffer() override
	{
		return pixbuf;
	}

private:
	GUI::PixelBufferAlpha pixbuf;
};

class PainterTest
	: public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(PainterTest);
	CPPUNIT_TEST(testDrawImage);
	CPPUNIT_TEST(testDrawText);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {}
	void tearDown() {}

	void testDrawImage()
	{
		// Success criterion is simply to not assert in the drawing routines...
		GUI::Image image(":logo.png");

		{ // Image fits in pixelbuffer
			TestableCanvas canvas(image.width(), image.height());
			GUI::Painter painter(canvas);
			painter.drawImage(0, 0, image);
		}

		{ // Image fits in pixelbuffer, negative offset
			TestableCanvas canvas(image.width(), image.height());
			GUI::Painter painter(canvas);
			painter.drawImage(-10, -10, image);
		}

		{ // Image too big for pixelbuffer
			TestableCanvas canvas(image.width() / 2, image.height() / 2);
			GUI::Painter painter(canvas);
			painter.drawImage(0, 0, image);
		}

		{ // Image fits in pixelbuffer but offset so it is drawn over the edge.
			TestableCanvas canvas(image.width(), image.height());
			GUI::Painter painter(canvas);
			painter.drawImage(10, 10, image);
		}

		{ // Image is offset to the right and down so nothing is to be drawn.
			TestableCanvas canvas(image.width(), image.height());
			GUI::Painter painter(canvas);
			painter.drawImage(image.width() + 1,
			                  image.height() + 1,
			                  image);
		}

		{ // Image is offset to the left and up so nothing is to be drawn.
			TestableCanvas canvas(image.width(), image.height());
			GUI::Painter painter(canvas);
			painter.drawImage(-1 * (image.width() + 1),
			                  -1 * (image.height() + 1),
			                  image);
		}
	}

	void testDrawText()
	{
		// Success criterion is simply to not assert in the drawing routines...
		GUI::Font font;
		// a string with unicode characters
		std::string someText = "Hello World - лæ Библиотека";
		std::size_t width = font.textWidth(someText);
		std::size_t height = font.textHeight(someText);

		{ // Text fits in pixelbuffer
			TestableCanvas canvas(width, height);
			GUI::Painter painter(canvas);
			painter.drawText(0, 0, font, someText);
		}

		{ // Text fits in pixelbuffer, negative offset
			TestableCanvas canvas(width, height);
			GUI::Painter painter(canvas);
			painter.drawText(-10, -10, font, someText);
		}

		{ // Text too big for pixelbuffer
			TestableCanvas canvas(width / 2, height / 2);
			GUI::Painter painter(canvas);
			painter.drawText(0, 0, font, someText);
		}

		{ // Text fits in pixelbuffer but offset so it is drawn over the edge.
			TestableCanvas canvas(width, height);
			GUI::Painter painter(canvas);
			painter.drawText(10, 10, font, someText);
		}

		{ // Text is offset to the right and down so nothing is to be drawn.
			TestableCanvas canvas(width, height);
			GUI::Painter painter(canvas);
			painter.drawText(width + 1,
			                 height + 1,
			                 font, someText);
		}

		{ // Text is offset to the left and up so nothing is to be drawn.
			TestableCanvas canvas(width, height);
			GUI::Painter painter(canvas);
			painter.drawText(-1 * (width + 1),
			                 -1 * (height + 1),
			                 font, someText);
		}
	}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(PainterTest);

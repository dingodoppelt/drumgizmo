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
#include <uunit.h>

#include <dggui/canvas.h>
#include <dggui/painter.h>
#include <dggui/image.h>
#include <dggui/font.h>

class TestColour
{
public:
	TestColour(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a)
		: colour(r, g, b, a) {}
	TestColour(const dggui::Colour& colour)
		: colour(colour) {}

	bool operator!=(const TestColour& other) const
	{
		return
			colour.red() != other.colour.red() ||
			colour.green() != other.colour.green() ||
			colour.blue() != other.colour.blue() ||
			colour.alpha() != other.colour.alpha()
			;
	}

	const dggui::Colour colour;
};

std::ostream& operator<<(std::ostream& stream, const TestColour& col)
{
	stream << "(" <<
		static_cast<int>(col.colour.red()) << ", " <<
		static_cast<int>(col.colour.green()) << ", " <<
		static_cast<int>(col.colour.blue()) << ", " <<
		static_cast<int>(col.colour.alpha()) << ")";
	return stream;
}

class TestableCanvas
	: public dggui::Canvas
{
public:
	TestableCanvas(std::size_t width, std::size_t height)
		: pixbuf(width, height)
	{}

	dggui::PixelBufferAlpha& getPixelBuffer() override
	{
		return pixbuf;
	}

private:
	dggui::PixelBufferAlpha pixbuf;
};

class TestImage
	: public dggui::Image
{
public:
	TestImage(std::uint8_t width, std::uint8_t height, bool alpha)
		: dggui::Image(":resources/logo.png") // just load some default image
	{
		_width = width;
		_height = height;
		has_alpha = alpha;

		image_data.resize(_width * _height);
		image_data_raw.resize(_width * _height);

		// Store x and y coordinates as red and green colour components
		for(std::uint8_t x = 0; x < _width; ++x)
		{
			for(std::uint8_t y = 0; y < _height; ++y)
			{
				image_data[x + _width * y] = dggui::Colour(x, y, 0, alpha ? 128 : 255);
				image_data_raw[4 * (x + _width * y) + 0] = x;
				image_data_raw[4 * (x + _width * y) + 1] = y;
				image_data_raw[4 * (x + _width * y) + 2] = 0;
				image_data_raw[4 * (x + _width * y) + 3] = alpha ? 128 : 255;
			}
		}

		valid = true;
	}
};

class PainterTest
	: public uUnit
{
public:
	PainterTest()
	{
		uUNIT_TEST(PainterTest::testDrawImage);
		uUNIT_TEST(PainterTest::testDrawText);
		uUNIT_TEST(PainterTest::testClipping);
	}

	void testDrawImage()
	{
		// Success criterion is simply to not assert in the drawing routines...
		dggui::Image image(":resources/logo.png");

		{ // Image fits in pixelbuffer
			TestableCanvas canvas(image.width(), image.height());
			dggui::Painter painter(canvas);
			painter.drawImage(0, 0, image);
		}

		{ // Image fits in pixelbuffer, negative offset
			TestableCanvas canvas(image.width(), image.height());
			dggui::Painter painter(canvas);
			painter.drawImage(-10, -10, image);
		}

		{ // Image too big for pixelbuffer
			TestableCanvas canvas(image.width() / 2, image.height() / 2);
			dggui::Painter painter(canvas);
			painter.drawImage(0, 0, image);
		}

		{ // Image fits in pixelbuffer but offset so it is drawn over the edge.
			TestableCanvas canvas(image.width(), image.height());
			dggui::Painter painter(canvas);
			painter.drawImage(10, 10, image);
		}

		{ // Image is offset to the right and down so nothing is to be drawn.
			TestableCanvas canvas(image.width(), image.height());
			dggui::Painter painter(canvas);
			painter.drawImage(image.width() + 1,
			                  image.height() + 1,
			                  image);
		}

		{ // Image is offset to the left and up so nothing is to be drawn.
			TestableCanvas canvas(image.width(), image.height());
			dggui::Painter painter(canvas);
			painter.drawImage(-1 * (image.width() + 1),
			                  -1 * (image.height() + 1),
			                  image);
		}
	}

	void testDrawText()
	{
		// Success criterion is simply to not assert in the drawing routines...
		dggui::Font font;
		// a string with unicode characters
		std::string someText = "Hello World - лæ Библиотека";
		std::size_t width = font.textWidth(someText);
		std::size_t height = font.textHeight(someText);

		{ // Text fits in pixelbuffer
			TestableCanvas canvas(width, height);
			dggui::Painter painter(canvas);
			painter.drawText(0, 0, font, someText);
		}

		{ // Text fits in pixelbuffer, negative offset
			TestableCanvas canvas(width, height);
			dggui::Painter painter(canvas);
			painter.drawText(-10, -10, font, someText);
		}

		{ // Text too big for pixelbuffer
			TestableCanvas canvas(width / 2, height / 2);
			dggui::Painter painter(canvas);
			painter.drawText(0, 0, font, someText);
		}

		{ // Text fits in pixelbuffer but offset so it is drawn over the edge.
			TestableCanvas canvas(width, height);
			dggui::Painter painter(canvas);
			painter.drawText(10, 10, font, someText);
		}

		{ // Text is offset to the right and down so nothing is to be drawn.
			TestableCanvas canvas(width, height);
			dggui::Painter painter(canvas);
			painter.drawText(width + 1,
			                 height + 1,
			                 font, someText);
		}

		{ // Text is offset to the left and up so nothing is to be drawn.
			TestableCanvas canvas(width, height);
			dggui::Painter painter(canvas);
			painter.drawText(-1 * (width + 1),
			                 -1 * (height + 1),
			                 font, someText);
		}
	}

	// Test rendering images outside the container is being clipped correctly.
	void testClipping()
	{
		TestableCanvas canvas(100, 100);
		dggui::Painter painter(canvas);

		{ // Without alpha
			TestImage image(16, 16, false);
			painter.clear();
			painter.drawImage(-10, -10, image);
			auto& pixbuf = canvas.getPixelBuffer();

			// Top left corner pixel should have the RGBA value (10, 10, 0, 255)
			uUNIT_ASSERT_EQUAL(TestColour(10, 10, 0, 255),
			                   TestColour(pixbuf.pixel(0, 0)));
		}

		{ // With alpha (different pipeline)
			TestImage image(16, 16, true);
			painter.clear();
			painter.drawImage(-10, -10, image);
			auto& pixbuf = canvas.getPixelBuffer();

			// Top left corner pixel should have the RGBA value (10, 10, 0, 128)
			uUNIT_ASSERT_EQUAL(TestColour(10, 10, 0, 128),
			                   TestColour(pixbuf.pixel(0, 0)));
		}
	}
};

// Registers the fixture into the 'registry'
static PainterTest test;

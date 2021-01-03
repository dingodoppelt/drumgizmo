/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            painter.cc
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
#include "painter.h"

#include <cmath>
#include <cassert>

#include "pixelbuffer.h"
#include "font.h"
#include "drawable.h"
#include "image.h"
#include "canvas.h"

namespace dggui
{

Painter::Painter(Canvas& canvas)
	: pixbuf(canvas.getPixelBuffer())
{
	colour = Colour(0.0f, 0.0f, 0.0f, 0.5f);
}

Painter::~Painter()
{
}

void Painter::setColour(const Colour& colour)
{
	this->colour = colour;
}

static void plot(PixelBufferAlpha& pixbuf, const Colour& colour,
                 int x, int y, double c)
{
	if((x >= (int)pixbuf.width) ||
	   (y >= (int)pixbuf.height) ||
	   (x < 0) ||
	   (y < 0))
	{
		return;
	}

	// plot the pixel at (x, y) with brightness c (where 0 ≤ c ≤ 1)
	Colour col(colour);
	if(c != 1)
	{
		col.data()[3] *= c;
	}
	pixbuf.addPixel(x, y, col);
}

static inline double fpart(double x)
{
	return x - std::floor(x);// fractional part of x
}

static inline double rfpart(double x)
{
	return 1 - fpart(x); // reverse fractional part of x
}

void Painter::drawLine(int x0, int y0, int x1, int y1)
{
	bool steep = abs(y1 - y0) > abs(x1 - x0);

	if(steep)
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	if(x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	double dx = x1 - x0;
	double dy = y1 - y0;
	double gradient = dy / dx;

	// Handle first endpoint:
	double xend = std::round(x0);
	double yend = y0 + gradient * (xend - x0);

	double xpxl1 = xend;   // this will be used in the main loop
	double ypxl1 = std::floor(yend);

	if(steep)
	{
		pixbuf.addPixel(ypxl1, xpxl1, colour);
	}
	else
	{
		pixbuf.addPixel(xpxl1, ypxl1, colour);
	}

	double intery = yend + gradient; // first y-intersection for the main loop

	// Handle second endpoint:
	xend = std::round(x1);
	yend = y1 + gradient * (xend - x1);

	double xpxl2 = xend; // this will be used in the main loop
	double ypxl2 = std::floor(yend);

	if(steep)
	{
		pixbuf.addPixel(ypxl2, xpxl2, colour);
	}
	else
	{
		pixbuf.addPixel(xpxl2, ypxl2, colour);
	}

	// main loop
	for(int x = xpxl1 + 1; x <= xpxl2 - 1; ++x)
	{
		if(steep)
		{
			plot(pixbuf, colour, std::floor(intery)  , x, rfpart(intery));
			plot(pixbuf, colour, std::floor(intery)+1, x,  fpart(intery));
		}
		else
		{
			plot(pixbuf, colour, x, std::floor(intery),  rfpart(intery));
			plot(pixbuf, colour, x, std::floor(intery)+1, fpart(intery));
		}
		intery += gradient;
	}
}

void Painter::drawRectangle(int x1, int y1, int x2, int y2)
{
	drawLine(x1, y1, x2 - 1, y1);
	drawLine(x2, y1, x2, y2 - 1);
	drawLine(x1 + 1, y2, x2, y2);
	drawLine(x1, y1 + 1, x1, y2);
}

void Painter::drawFilledRectangle(int x1, int y1, int x2, int y2)
{
	for(int y = y1; y <= y2; ++y)
	{
		drawLine(x1, y, x2, y);
	}
}

void Painter::clear()
{
	pixbuf.clear();
}

void Painter::drawText(int x0, int y0, const Font& font,
                       const std::string& text, bool nocolour, bool rotate)
{
	PixelBufferAlpha* textbuf = font.render(text);

	if(!rotate)
	{
		y0 -= textbuf->height; // The y0 offset (baseline) is the bottom of the text.
	}

	// If the text offset is outside the buffer; skip it.
	if((x0 > (int)pixbuf.width) || (y0 > (int)pixbuf.height))
	{
		delete textbuf;
		return;
	}

	// Make sure we don't try to draw outside the pixbuf.
	int renderWidth = textbuf->width;
	if(renderWidth > (int)(pixbuf.width - x0))
	{
		renderWidth = pixbuf.width - x0;
	}

	int renderHeight = textbuf->height;
	if(renderHeight > ((int)pixbuf.height - y0))
	{
		renderHeight = ((int)pixbuf.height - y0);
	}

	if(nocolour)
	{
		for(int y = -1 * std::min(0, y0); y < renderHeight; ++y)
		{
			int x = -1 * std::min(0, x0);

			assert(x >= 0);
			assert(y >= 0);
			assert(x < (int)textbuf->width);
			assert(y < (int)textbuf->height);

			auto c = textbuf->getLine(x, y);

			assert(x + x0 >= 0);
			assert(y + y0 >= 0);
			assert(x + x0 < (int)pixbuf.width);
			assert(y + y0 < (int)pixbuf.height);

			pixbuf.blendLine(x + x0, y + y0, c, renderWidth - x);
		}
	}
	else if(rotate)
	{
		int renderWidth = textbuf->height;
		if(renderWidth > (int)(pixbuf.width - x0))
		{
			renderWidth = pixbuf.width - x0;
		}

		int renderHeight = textbuf->width;
		if(renderHeight > ((int)pixbuf.height - y0))
		{
			renderHeight = ((int)pixbuf.height - y0);
		}

		for(int y = -1 * std::min(0, y0); y < renderHeight; ++y)
		{
			for(int x = -1 * std::min(0, x0); x < renderWidth; ++x)
			{
				assert(x >= 0);
				assert(y >= 0);
				assert(x < (int)textbuf->height);
				assert(y < (int)textbuf->width);

				auto c = textbuf->pixel(textbuf->width - y - 1, x);

				assert(x + x0 >= 0);
				assert(y + y0 >= 0);
				assert(x + x0 < (int)pixbuf.width);
				assert(y + y0 < (int)pixbuf.height);

				Colour col(colour.red(), colour.green(),
				           colour.blue(), (int)(colour.alpha() * c.alpha()) / 255);
				pixbuf.addPixel(x + x0, y + y0, col);
			}
		}
	}
	else
	{
		for(int y = -1 * std::min(0, y0); y < renderHeight; ++y)
		{
			for(int x = -1 * std::min(0, x0); x < renderWidth; ++x)
			{
				assert(x >= 0);
				assert(y >= 0);
				assert(x < (int)textbuf->width);
				assert(y < (int)textbuf->height);

				auto c = textbuf->pixel(x, y);

				assert(x + x0 >= 0);
				assert(y + y0 >= 0);
				assert(x + x0 < (int)pixbuf.width);
				assert(y + y0 < (int)pixbuf.height);

				Colour col(colour.red(), colour.green(),
				           colour.blue(), (int)(colour.alpha() * c.alpha()) / 255);
				pixbuf.addPixel(x + x0, y + y0, col);
			}
		}
	}

	delete textbuf;
}

void Painter::drawPoint(int x, int y)
{
	if(x >= 0 && y >= 0 && (std::size_t)x < pixbuf.width && (std::size_t)y < pixbuf.height)
	{
		pixbuf.setPixel(x, y, colour);
	}
}

static void plot4points(Painter *p, int cx, int cy, int x, int y)
{
	p->drawPoint(cx + x, cy + y);
	if(x != 0)
	{
		p->drawPoint(cx - x, cy + y);
	}

	if(y != 0)
	{
		p->drawPoint(cx + x, cy - y);
	}

	if(x != 0 && y != 0)
	{
		p->drawPoint(cx - x, cy - y);
	}
}

void Painter::drawCircle(int cx, int cy, double radius)
{
	int error = -radius;
	int x = radius;
	int y = 0;

	while(x >= y)
	{
		plot4points(this, cx, cy, x, y);

		if(x != y)
		{
			plot4points(this, cx, cy, y, x);
		}

		error += y;
		++y;
		error += y;

		if(error >= 0)
		{
			--x;
			error -= x;
			error -= x;
		}
	}
}

static void plot4lines(Painter *p, int cx, int cy, int x, int y)
{
	p->drawLine(cx + x, cy + y, cx - x, cy + y);
	if(x != 0)
	{
		p->drawLine(cx - x, cy + y, cx + x, cy + y);
	}

	if(y != 0)
	{
		p->drawLine(cx + x, cy - y, cx - x, cy - y);
	}

	if(x != 0 && y != 0)
	{
		p->drawLine(cx - x, cy - y, cx + x, cy - y);
	}
}

void Painter::drawFilledCircle(int cx, int cy, int radius)
{
	int error = -radius;
	int x = radius;
	int y = 0;

	while(x >= y)
	{
		plot4lines(this, cx, cy, x, y);

		if(x != y)
		{
			plot4lines(this, cx, cy, y, x);
		}

		error += y;
		++y;
		error += y;

		if(error >= 0)
		{
			--x;
			error -= x;
			error -= x;
		}
	}
}

void Painter::drawImage(int x0, int y0, const Drawable& image)
{
	int fw = image.width();
	int fh = image.height();

	// Make sure we don't try to draw outside the pixbuf.
	if(fw > (int)(pixbuf.width - x0))
	{
		fw = (int)(pixbuf.width - x0);
	}

	if(fh > (int)(pixbuf.height - y0))
	{
		fh = (int)(pixbuf.height - y0);
	}

	if((fw < 1) || (fh < 1))
	{
		return;
	}

	if(image.hasAlpha())
	{
		if(!image.line(0))
		{
			for(std::size_t y = -1 * std::min(0, y0); y < (std::size_t)fh; ++y)
			{
				for(std::size_t x = -1 * std::min(0, x0); x < (std::size_t)fw; ++x)
				{
					assert(x >= 0);
					assert(y >= 0);
					assert(x < image.width());
					assert(y < image.height());
					auto& c = image.getPixel(x, y);

					assert(x0 + x >= 0);
					assert(y0 + y >= 0);
					assert(x0 + x < pixbuf.width);
					assert(y0 + y < pixbuf.height);

					pixbuf.addPixel(x0 + x, y0 + y, c);
				}
			}
		}
		else
		{
			std::size_t x_offset = -1 * std::min(0, x0);
			for(std::size_t y = -1 * std::min(0, y0); y < (std::size_t)fh; ++y)
			{
				pixbuf.blendLine(x_offset + x0, y + y0, image.line(y, x_offset),
				                 std::min((int)image.width(), fw - (int)x_offset));
			}
		}
	}
	else
	{
		std::size_t x_offset = -1 * std::min(0, x0);
		for(std::size_t y = -1 * std::min(0, y0); y < (std::size_t)fh; ++y)
		{
			pixbuf.writeLine(x_offset + x0, y + y0, image.line(y, x_offset),
			                 std::min((int)image.width(), fw - (int)x_offset));
		}
	}
}

void Painter::drawRestrictedImage(int x0, int y0,
                                  const Colour& restriction_colour,
                                  const Drawable& image)
{
	int fw = image.width();
	int fh = image.height();

	// Make sure we don't try to draw outside the pixbuf.
	if(fw > (int)(pixbuf.width - x0))
	{
		fw = (int)(pixbuf.width - x0);
	}

	if(fh > (int)(pixbuf.height - y0))
	{
		fh = (int)(pixbuf.height - y0);
	}

	if((fw < 1) || (fh < 1))
	{
		return;
	}

	for(std::size_t y = -1 * std::min(0, y0); y < (std::size_t)fh; ++y)
	{
		for(std::size_t x = -1 * std::min(0, x0); x < (std::size_t)fw; ++x)
		{
			assert(x >= 0);
			assert(y >= 0);
			assert(x < image.width());
			assert(y < image.height());
			auto& c = image.getPixel(x, y);

			assert(x0 + x >= 0);
			assert(y0 + y >= 0);
			assert(x0 + x < pixbuf.width);
			assert(y0 + y < pixbuf.height);

			if(c == restriction_colour)
			{
				pixbuf.setPixel(x0 + x, y0 + y, c);
			}
		}
	}
}

void Painter::drawImageStretched(int x0, int y0, const Drawable& image,
                                 int width, int height)
{
	float fw = image.width();
	float fh = image.height();

	// Make sure we don't try to draw outside the pixbuf.
	if(width > (int)(pixbuf.width - x0))
	{
		width = pixbuf.width - x0;
	}

	if(height > (int)(pixbuf.height - y0))
	{
		height = pixbuf.height - y0;
	}

	if((width < 1) || (height < 1))
	{
		return;
	}

	for(int y = -1 * std::min(0, y0); y < height; ++y)
	{
		for(int x = -1 * std::min(0, x0); x < width; ++x)
		{
			int lx = ((float)x / (float)width) * fw;
			int ly = ((float)y / (float)height) * fh;
			auto& c = image.getPixel(lx, ly);
			pixbuf.addPixel(x0 + x, y0 + y, c);
		}
	}
}

void Painter::drawBox(int x, int y, const Box& box, int width, int height)
{
	int dx = x;
	int dy = y;

	// Top:
	drawImage(dx, dy, *box.topLeft);

	dx += box.topLeft->width();
	if((dx < 0) || (dy < 0))
	{
		return;
	}

	drawImageStretched(dx, dy, *box.top,
	                   width - box.topRight->width() - box.topLeft->width(),
	                   box.top->height());

	dx = x + width - box.topRight->width();
	if((dx < 0) || (dy < 0))
	{
		return;
	}

	drawImage(dx, dy, *box.topRight);

	// Center:
	dy = y + box.topLeft->height();
	dx = x + box.left->width();
	if((dx < 0) || (dy < 0))
	{
		return;
	}

	drawImageStretched(dx, dy, *box.center,
	                   width - box.left->width() - box.right->width(),
	                   height - box.topLeft->height() - box.bottomLeft->height());

	// Mid:
	dx = x;
	dy = y + box.topLeft->height();
	if((dx < 0) || (dy < 0))
	{
		return;
	}

	drawImageStretched(dx, dy, *box.left, box.left->width(),
	                   height - box.topLeft->height() - box.bottomLeft->height());

	dx = x + width - box.right->width();
	dy = y + box.topRight->height();
	if((dx < 0) || (dy < 0))
	{
		return;
	}

	drawImageStretched(dx, dy, *box.right,
	                   box.right->width(),
	                   height - box.topRight->height() - box.bottomRight->height());

	// Bottom:
	dx = x;
	dy = y + height - box.bottomLeft->height();
	if((dx < 0) || (dy < 0))
	{
		return;
	}

	drawImage(dx, dy, *box.bottomLeft);

	dx += box.bottomLeft->width();
	if((dx < 0) || (dy < 0))
	{
		return;
	}

	drawImageStretched(dx, dy, *box.bottom,
	                   width - box.bottomRight->width() - box.bottomLeft->width(),
	                   box.bottom->height());

	dx = x + width - box.bottomRight->width();
	if((dx < 0) || (dy < 0))
	{
		return;
	}

	drawImage(dx, dy, *box.bottomRight);
}

void Painter::drawBar(int x, int y, const Bar& bar, int width, int height)
{
	if(width < ((int)bar.left->width() + (int)bar.right->width() + 1))
	{
		width = bar.left->width() + bar.right->width() + 1;
	}

	drawImageStretched(x, y, *bar.left, bar.left->width(), height);

	drawImageStretched(x + bar.left->width(), y, *bar.center,
	                   width - bar.left->width() - bar.right->width(), height);

	drawImageStretched(x + width - bar.left->width(), y, *bar.right,
	                   bar.right->width(), height);
}

} // dggui::

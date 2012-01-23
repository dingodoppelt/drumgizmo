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
#include "painter.h"

#include "window.h"

#include <string.h>

GUI::Painter::Painter(GUI::Widget *widget)
{
  this->widget = widget;
  widget->window()->beginPaint();
  pixbuf = &widget->pixbuf;
  colour = Colour(0, 0, 0, 0.5);
}

GUI::Painter::~Painter()
{
  widget->window()->endPaint();
  flush();
}

void GUI::Painter::setColour(Colour colour)
{
  this->colour = colour;
}

void GUI::Painter::drawLine(int x0, int y0, int x1, int y1)
{
  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);

  int sx;
  if(x0 < x1) sx = 1;
  else sx = -1;

  int sy;
  if(y0 < y1) sy = 1;
  else sy = -1;

  int err = dx-dy;
 
  while(true) {
    drawPoint(x0, y0);
    if(x0 == x1 && y0 == y1) break;
    int e2 = 2 * err;
    if(e2 > -dy) {
      err -= dy;
      x0 += sx;
    }
    if(e2 < dx) {
      err += dx;
      y0 += sy;
    }
  }
}

void GUI::Painter::drawRectangle(int x1, int y1, int x2, int y2)
{
  drawLine(x1, y1, x2, y1);
  drawLine(x2, y1, x2, y2);
  drawLine(x1, y2, x2, y2);
  drawLine(x1, y1, x1, y2);
}

void GUI::Painter::drawFilledRectangle(int x1, int y1, int x2, int y2)
{
  for(int x = x1; x < x2; x++) {
    for(int y = y1; y < y2; y++) {
      drawPoint(x, y);
    }
  }
}

void GUI::Painter::drawText(int x0, int y0, GUI::Font &font, std::string text)
{
  PixelBufferAlpha *textbuf = font.render(widget->window()->gctx, text);

  for(size_t x = 0; x < textbuf->width; x++) {
    for(size_t y = 0; y < textbuf->height; y++) {
      unsigned char r,g,b,a;
      textbuf->pixel(x, y, &r, &g, &b, &a);
      if(a) drawPoint(x + x0, y + y0 - textbuf->height);
    }
  }

  delete textbuf;
  
}

#include <stdio.h>
void GUI::Painter::drawPoint(int x, int y)
{
  //  printf("Painter::drawPoint: green %f\n", colour.green); fflush(stdout);
  pixbuf->setPixel(x, y,
                   (unsigned char)(colour.red * 255.0),
                   (unsigned char)(colour.green * 255.0),
                   (unsigned char)(colour.blue * 255.0),
                   (unsigned char)(colour.alpha * 255.0));
}

static void plot4points(GUI::Painter *p, int cx, int cy, int x, int y)
{
  p->drawPoint(cx + x, cy + y);
  if(x != 0) p->drawPoint(cx - x, cy + y);
  if(y != 0) p->drawPoint(cx + x, cy - y);
  if(x != 0 && y != 0) p->drawPoint(cx - x, cy - y);
}

void GUI::Painter::drawCircle(int cx, int cy, int radius)
{
  int error = -radius;
  int x = radius;
  int y = 0;
 
  while(x >= y) {
    plot4points(this, cx, cy, x, y);
    if(x != y) plot4points(this, cx, cy, y, x);
 
    error += y;
    ++y;
    error += y;
 
    if(error >= 0) {
      --x;
      error -= x;
      error -= x;
    }
  }
}

static void plot4lines(GUI::Painter *p, int cx, int cy, int x, int y)
{
  p->drawLine(cx + x, cy + y, cx - x, cy + y);
  if(x != 0) p->drawLine(cx - x, cy + y, cx + x, cy + y);
  if(y != 0) p->drawLine(cx + x, cy - y, cx - x, cy - y);
  if(x != 0 && y != 0) p->drawLine(cx - x, cy - y, cx + x, cy - y);
}

void GUI::Painter::drawFilledCircle(int cx, int cy, int radius)
{
  int error = -radius;
  int x = radius;
  int y = 0;
 
  while(x >= y) {
    plot4lines(this, cx, cy, x, y);
    if(x != y) plot4lines(this, cx, cy, y, x);
 
    error += y;
    ++y;
    error += y;
 
    if(error >= 0) {
      --x;
      error -= x;
      error -= x;
    }
  }
}

void GUI::Painter::drawImage(int x0, int y0, struct __img__ * img)
{
  size_t fw = img->width;

  for(size_t x = 0; x < fw; x++) {
    for(size_t y = 0; y < img->height; y++) {
      unsigned int pixel = img->pixels[x + y * fw];
      unsigned int order = img->order;
      unsigned char *c = (unsigned char *)&pixel;
      unsigned char *o = (unsigned char *)&order;

      pixbuf->setPixel(x0 + x, y0 + y, c[o[1]], c[o[2]], c[o[3]], c[o[0]]);
    }
  }
}

void GUI::Painter::flush()
{
#ifdef X11
  // Send the "DrawLine" request to the server
  //XFlush(gctx->display);
#endif/*X11*/
}

#ifdef TEST_PAINTER
//Additional dependency files
//deps:
//Required cflags (autoconf vars may be used)
//cflags:
//Required link options (autoconf vars may be used)
//libs:
#include "test.h"

TEST_BEGIN;

// TODO: Put some testcode here (see test.h for usable macros).

TEST_END;

#endif/*TEST_PAINTER*/

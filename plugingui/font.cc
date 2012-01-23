/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            font.cc
 *
 *  Sat Nov 12 11:13:41 CET 2011
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
#include "font.h"

#ifdef X11
#include <X11/Xlib.h>
#endif/*X11*/

#include "img_font.h"

GUI::Font::Font()
{
}
  
void GUI::Font::setFace(std::string face)
{
  face = face;
}

std::string GUI::Font::face()
{
  return "";
}

void GUI::Font::setSize(size_t points)
{
  points = points;
}

size_t GUI::Font::size()
{
  return 0;
}

size_t GUI::Font::textWidth(std::string text)
{
  size_t fw = img_font.width / 255;
  return fw * text.length(); 
}

size_t GUI::Font::textHeight(std::string text)
{
  return img_font.height;
}

GUI::PixelBufferAlpha *GUI::Font::render(GlobalContext *gctx, std::string text)
{
  PixelBufferAlpha *pb =
    new PixelBufferAlpha(textWidth(text), textHeight(text));
  size_t fw = img_font.width / 255;
  for(size_t i = 0; i < text.length(); i++) {
    unsigned int cha = text[i];
    for(size_t x = 0; x < fw; x++) {
      for(size_t y = 0; y < img_font.height; y++) {
        unsigned int pixel = img_font.pixels[(x + (cha * fw)) + y * img_font.width];
        unsigned int order = img_font.order;
        unsigned char *c = (unsigned char *)&pixel;
        unsigned char *o = (unsigned char *)&order;

        pb->setPixel(x + i * (fw), y,
                     c[o[1]], c[o[2]], c[o[3]], c[o[0]]);
      }
    }
  }

  /*
  for(size_t i = 0; i < text.length(); i++) {
    for(size_t x = 0; x < WIDTH; x++) {
      for(size_t y = 0; y < HEIGHT; y++) {
        pb->setPixel(x + i * (WIDTH + BORDER), y, 255, 255, 255, 255);
      }
    }
  }
  */
#ifdef X11
  /*
  XTextItem item;
  item.chars = (char*)text.data();
  item.nchars = text.length();
  item.delta = 0;
  item.font = None;

  Pixmap pixmap = XCreatePixmap(gctx->display, NULL,
                                textWidth(text), textHeight(text), 24);
  GC gc = XCreateGC(gctx->display, pixmap, 0, NULL);
  XDrawText(gctx->display, pixmap, gc, 0, 0, &item, 1);
  */
#endif/*X11*/

  return pb;
}

#ifdef TEST_FONT
//deps:
//cflags:
//libs:
#include "test.h"

TEST_BEGIN;

// TODO: Put some testcode here (see test.h for usable macros).

TEST_END;

#endif/*TEST_FONT*/

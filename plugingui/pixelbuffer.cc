/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            pixelbuffer.cc
 *
 *  Thu Nov 10 09:00:38 CET 2011
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
#include "pixelbuffer.h"

#include <stdio.h>

GUI::PixelBuffer::PixelBuffer(size_t width, size_t height)
{
  buf = NULL;
  realloc(width, height);
}

void GUI::PixelBuffer::realloc(size_t width, size_t height)
{
  if(buf) free(buf);
  buf = (unsigned char *)calloc(width * height, 3);
  this->width = width;
  this->height = height;
}

#define PX(k) (x + y * width) * 3 + k
void GUI::PixelBuffer::setPixel(size_t x, size_t y,
                                unsigned char red,
                                unsigned char green,
                                unsigned char blue,
                                unsigned char alpha)
{
  //printf("%d %d %d\n", red, green, blue);
  if(x >= width || y >= height) return;
  float a = alpha / 255.0;
  buf[PX(0)] = (unsigned char)(red * a + buf[PX(0)] * (1-a));
  buf[PX(1)] = (unsigned char)(green * a + buf[PX(1)] * (1-a));
  buf[PX(2)] = (unsigned char)(blue * a + buf[PX(2)] * (1-a));
}

static int idx = 0;
GUI::PixelBufferAlpha::PixelBufferAlpha(size_t width, size_t height)
{
  this->idx = ::idx++;
  buf = NULL;
  x = y = 10;
  realloc(width, height);
}

void GUI::PixelBufferAlpha::realloc(size_t width, size_t height)
{
  if(buf) free(buf);
  buf = (unsigned char *)calloc(width * height, 4);
  this->width = width;
  this->height = height;
}

#undef PX
#define PX(k) (x + y * width) * 4 + k
void GUI::PixelBufferAlpha::setPixel(size_t x, size_t y,
                                     unsigned char red,
                                     unsigned char green,
                                     unsigned char blue,
                                     unsigned char alpha)
{
  //  printf("%d %d %d %d\n", red, green, blue, alpha);
  if(x >= width || y >= height) return;
  /*
  float a = alpha / 255.0;
  buf[PX(0)] = (unsigned char)(red * a + buf[PX(0)] * (1-a));
  buf[PX(1)] = (unsigned char)(green * a + buf[PX(1)] * (1-a));
  buf[PX(2)] = (unsigned char)(blue * a + buf[PX(2)] * (1-a));
  buf[PX(3)] = (unsigned char)(alpha * a + buf[PX(3)] * (1-a));
  */
  buf[PX(0)] = red;
  buf[PX(1)] = green;
  buf[PX(2)] = blue;
  buf[PX(3)] = alpha;
}

void GUI::PixelBufferAlpha::addPixel(size_t x, size_t y,
                                     unsigned char red,
                                     unsigned char green,
                                     unsigned char blue,
                                     unsigned char alpha)
{
  //  printf("%d %d %d %d\n", red, green, blue, alpha);
  if(x >= width || y >= height) return;

  if(alpha == 0) return;

  float a = (float)alpha / (float)(alpha + buf[PX(3)]);
  buf[PX(0)] = (unsigned char)(red * a + buf[PX(0)] * (1-a));
  buf[PX(1)] = (unsigned char)(green * a + buf[PX(1)] * (1-a));
  buf[PX(2)] = (unsigned char)(blue * a + buf[PX(2)] * (1-a));

  //buf[PX(3)] = (unsigned char)(alpha * a + buf[PX(3)] * (1-a));
  buf[PX(3)] = alpha>buf[PX(3)]?alpha:buf[PX(3)];
}

void GUI::PixelBufferAlpha::pixel(size_t x, size_t y,
                                  unsigned char *red,
                                  unsigned char *green,
                                  unsigned char *blue,
                                  unsigned char *alpha)
{
  //printf("%d %d %d\n", red, green, blue);
  if(x > width || y > height) return;
  *red = buf[PX(0)];
  *green = buf[PX(1)];
  *blue = buf[PX(2)];
  *alpha = buf[PX(3)];
}

#ifdef TEST_PIXELBUFFER
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

#endif/*TEST_PIXELBUFFER*/

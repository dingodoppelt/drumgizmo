/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            image.cc
 *
 *  Sat Mar 16 15:05:09 CET 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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
#include "image.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <hugin.hpp>

#include "resource.h"
// http://blog.hammerian.net/2009/reading-png-images-from-memory/

typedef struct {
  size_t p;
  size_t size;
  const char *data;
} data_io_t;

static void dio_reader(png_structp png_ptr, png_bytep buf, png_size_t size)
{
  data_io_t *dio = (data_io_t *)png_get_io_ptr(png_ptr);

  if(size > dio->size - dio->p) png_error(png_ptr, "Could not read bytes.");

  memcpy(buf, (dio->data + dio->p), size);
  dio->p += size;
}

GUI::Image::Image(const char* data, size_t size)
{
  load(data, size);
}

GUI::Image::Image(std::string filename)
{
  GUI::Resource rc(filename);
  load(rc.data(), rc.size());
}

GUI::Image::~Image()
{
}

void GUI::Image::setError(int err)
{
  GUI::Resource rc(":png_error");

  const char *p = rc.data();

  memcpy(&w, p, 4); p += 4;
  memcpy(&h, p, 4); p += 4;

  DEBUG(image, "w:%d, h:%d\n", w, h);

  row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * h);
  for(unsigned int y = 0; y < h; y++) {
    size_t size = w * sizeof(unsigned int);
    DEBUG(image, "rc.size:%d >= p:%d (rowsize: %d)\n",
          rc.size(), p - rc.data(), size);
    row_pointers[y] = (png_byte*)malloc(size);
    memcpy(row_pointers[y], p, size);
    p += size;
  }
}

void GUI::Image::load(const char* data, size_t size)
{
  const char *header = data;

  // test for it being a png:
  if(png_sig_cmp((png_byte*)header, 0, 8)) {
    ERR(image, "[read_png_file] File is not recognized as a PNG file");
    setError(0);
    return;
  }
  
  // initialize stuff
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  
  if(!png_ptr) {
    ERR(image, "[read_png_file] png_create_read_struct failed");
    setError(1);
    return;
  }
  
  info_ptr = png_create_info_struct(png_ptr);
  if(!info_ptr) {
    ERR(image, "[read_png_file] png_create_info_struct failed");
    setError(2);
    return;
  }
  
  if(setjmp(png_jmpbuf(png_ptr))) {
    ERR(image, "[read_png_file] Error during init_io");
    setError(3);
    return;
  }
  
  //png_init_io(png_ptr, fp);
  data_io_t dio;
  dio.data = data;
  dio.size = size;
  dio.p = 8; // skip header
  png_set_read_fn(png_ptr, &dio, dio_reader);

  png_set_sig_bytes(png_ptr, 8);
  
  png_read_info(png_ptr, info_ptr);
  
  w = png_get_image_width(png_ptr, info_ptr);
  h = png_get_image_height(png_ptr, info_ptr);
  color_type = png_get_color_type(png_ptr, info_ptr);
  bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  
  number_of_passes = png_set_interlace_handling(png_ptr);
  png_read_update_info(png_ptr, info_ptr);
  
  
  // read file
  if (setjmp(png_jmpbuf(png_ptr))) {
    ERR(image, "[read_png_file] Error during read_image");
    setError(4);
    return;
  }
  
  row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * h);
  size_t y;
  for(y = 0; y < h; y++) {
    row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
  }
  
  png_read_image(png_ptr, row_pointers);
}

size_t GUI::Image::width()
{
  return w;
}

size_t GUI::Image::height()
{
  return h;
}

GUI::Colour GUI::Image::getPixel(size_t x, size_t y)
{
  if(x > width() || y > height()) return GUI::Colour(0,0,0,0);
  png_byte* row = row_pointers[y];
  png_byte* ptr = &(row[x*4]);
  float r = ptr[0];
  float g = ptr[1];
  float b = ptr[2];
  float a = ptr[3];
  GUI::Colour c(r / 255.0,
                g / 255.0,
                b / 255.0,
                a / 255.0);
  return c;
}

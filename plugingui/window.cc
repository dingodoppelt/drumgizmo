/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            window.cc
 *
 *  Sun Oct  9 13:11:53 CEST 2011
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
#include "window.h"

#include "painter.h"

#include "img_back.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#ifdef X11
#include "nativewindow_x11.h"
#endif/*X11*/

#ifdef WIN32
#include "nativewindow_win32.h"
#endif/*WIN32*/

GUI::Window *gwindow = NULL;

GUI::Window::Window(GlobalContext *gctx) 
  : Widget(NULL), wpixbuf(640, 200)
{
  gwindow = this;
  
  this->gctx = gctx;

  _x = _y = 100;
  _width = wpixbuf.width;
  _height = wpixbuf.height;

  refcount = 0;
  _keyboardFocus = this;
  _buttonDownFocus = NULL;

#ifdef X11
  native = new NativeWindowX11(gctx, this);
#endif/*X11*/

#ifdef WIN32
  native = new NativeWindowWin32(gctx, this);
#endif/*WIN32*/
}

GUI::Window::~Window()
{
  delete native;
}

void GUI::Window::setCaption(std::string caption)
{
  native->setCaption(caption);
}

void GUI::Window::repaintEvent(GUI::RepaintEvent *e)
{
  Painter p(this);
  p.drawImage(0, 0, (struct __img__*)&img_back);
}

void GUI::Window::resize(size_t width, size_t height)
{
  //  printf("Window::resize(%d, %d)\n", width, height);
  native->resize(width, height);
  Widget::resize(width, height);
}

void GUI::Window::move(size_t x, size_t y)
{
  native->move(x, y);

  // Make sure widget corrds are updated.
  Widget::move(x, y);
}

size_t GUI::Window::x() { return _x; }
size_t GUI::Window::y() { return _y; }
size_t GUI::Window::width() { return _width; }
size_t GUI::Window::height() { return _height; }
size_t GUI::Window::windowX() { return 0; }
size_t GUI::Window::windowY() { return 0; }

void GUI::Window::show()
{
  repaint_r(NULL);
  native->show();
}

void GUI::Window::hide()
{
  native->hide();
}

GUI::Window *GUI::Window::window()
{
  return this;
}

void GUI::Window::beginPaint()
{
  refcount++;
  //  printf("beginPaint(%d)\n", refcount);
}

void GUI::Window::endPaint()
{
  //  printf("endPaint(%d)\n", refcount);
  if(refcount) refcount--;

  if(!refcount) {
    updateBuffer();
#ifdef X11
    //XSendEvent(gctx->display, window, false, ExposureMask, event_send)
#endif/*X11*/
    redraw();
  }
}

void GUI::Window::updateBuffer()
{
  //  printf("updateBuffer w:%d h:%d\n", width(), height());

  memset(wpixbuf.buf, 0, wpixbuf.width * wpixbuf.height * 3);

  std::vector<PixelBufferAlpha *> pl = getPixelBuffers();
  std::vector<PixelBufferAlpha *>::iterator pli = pl.begin();
  while(pli != pl.end()) {
    PixelBufferAlpha *pb = *pli;
    //    printf("Buffer idx %d (%d %d) [%d %d]\n", pb->idx, pb->x, pb->y,
    //       pb->width, pb->height);
    for(size_t x = 0; x < pb->width; x++) {
      for(size_t y = 0; y < pb->height; y++) {
        unsigned char r,g,b,a;
        pb->pixel(x,y,&r,&g,&b,&a);
        wpixbuf.setPixel(x + pb->x, y + pb->y, r, g, b, a);
      }
    }
    pli++;
  }
  native->handleBuffer();
}

void GUI::Window::resized(size_t w, size_t h)
{
  _width = w;
  _height = h;
  wpixbuf.realloc(w, h);
  updateBuffer();

  pixbuf.realloc(w, h);
  repaintEvent(NULL);
}

void GUI::Window::redraw()
{
  native->redraw();
}

GUI::Widget *GUI::Window::keyboardFocus()
{
  return _keyboardFocus;
}

void GUI::Window::setKeyboardFocus(GUI::Widget *widget)
{
  _keyboardFocus = widget;
  repaint_r(NULL);
}

GUI::Widget *GUI::Window::buttonDownFocus()
{
  return _buttonDownFocus;
}

void GUI::Window::setButtonDownFocus(GUI::Widget *widget)
{
  _buttonDownFocus = widget;
  native->grabMouse(widget != NULL);
  //  repaint_r(NULL);
}

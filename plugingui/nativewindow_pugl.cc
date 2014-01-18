/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            nativewindow_pugl.cc
 *
 *  Fri Dec 28 18:45:57 CET 2012
 *  Copyright 2012 Bent Bisballe Nyeng
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
#include "nativewindow_pugl.h"

#include <stdlib.h>
#include <list>

#include "hugin.hpp"
#include "guievent.h"

static GUI::Window* windowptr;
static std::list<GUI::Event*> eventq;

static void onDisplay(PuglView* view)
{

  glDisable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT);

  GLuint image;

  glGenTextures(1, &image);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_NEAREST = no smoothing
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowptr->wpixbuf.width,
                windowptr->wpixbuf.height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                windowptr->wpixbuf.buf);

  glEnable(GL_TEXTURE_2D);
  
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0); glVertex2f(0.0, 0.0);
  glTexCoord2d(0.0, 1.0); glVertex2f(0.0, windowptr->wpixbuf.height);
  glTexCoord2d(1.0, 1.0); glVertex2f(windowptr->wpixbuf.width, windowptr->wpixbuf.height);
  glTexCoord2d(1.0, 0.0); glVertex2f(windowptr->wpixbuf.width, 0.0);
  glEnd();

  glDeleteTextures(1, &image);
  glDisable(GL_TEXTURE_2D);
  glFlush();
  
  puglPostRedisplay(view);
}

static void onMouse(PuglView* view, int button, bool press, int x, int y)
{
  DEBUG(nativewindow_pugl, "Mouse %d %s at (%d,%d)\n", button,
                            press? "down":"up", x, y);

  GUI::ButtonEvent* e = new GUI::ButtonEvent();
  e->x = x;
  e->y = y;
  e->button = button;
  e->direction = press?1:-1;
  e->doubleclick = false; 

  eventq.push_back(e);
}

static void onKeyboard(PuglView* view, bool press, uint32_t key) 
{
  if(press) {
    GUI::KeyEvent* e = new GUI::KeyEvent();
    e->direction = press?-1:1;

    printf("%d\n", key);
 
    switch(key) {
      case PUGL_KEY_LEFT: e->keycode = GUI::KeyEvent::KEY_LEFT; break;
      case PUGL_KEY_RIGHT: e->keycode = GUI::KeyEvent::KEY_RIGHT; break;
      case PUGL_KEY_UP: e->keycode = GUI::KeyEvent::KEY_UP; break;
      case PUGL_KEY_DOWN: e->keycode = GUI::KeyEvent::KEY_DOWN; break;
      case PUGL_KEY_PAGE_UP: e->keycode = GUI::KeyEvent::KEY_PGDOWN; break;
      case PUGL_KEY_PAGE_DOWN: e->keycode = GUI::KeyEvent::KEY_PGUP; break;
      default: e->keycode = GUI::KeyEvent::KEY_UNKNOWN; break;
    }

    // TODO: perform character type check
    if(e->keycode == GUI::KeyEvent::KEY_UNKNOWN) {
      e->keycode = GUI::KeyEvent::KEY_CHARACTER;
      e->text.assign(1, (char)key); 
    }

    printf("\t text: %s\n", e->text.c_str());
    
    eventq.push_back(e);
  }
}

GUI::NativeWindowPugl::NativeWindowPugl(GUI::Window *window)
  : GUI::NativeWindow()
{
  INFO(nativewindow, "Running with PuGL native window\n");
  this->window = window;
  windowptr = window;
  view = NULL;
  init();
}

GUI::NativeWindowPugl::~NativeWindowPugl()
{
  puglDestroy(view);
}

void GUI::NativeWindowPugl::init() {
  PuglView* old = view;
  if(view) old = view;
//  view = puglCreate(0, "DrumgGizmo", window->x(), window->y(), false, true);
  view = puglCreate(0, "DrumgGizmo", 370, 330, false, true);
  puglSetDisplayFunc(view, onDisplay);
  puglSetMouseFunc(view, onMouse);
  puglSetKeyboardFunc(view, onKeyboard);

  if(old) free(old);
}

void GUI::NativeWindowPugl::setFixedSize(int width, int height)
{
//  redraw();
}

void GUI::NativeWindowPugl::resize(int width, int height)
{
//  DEBUG(nativewindow_pugl, "Resizing to %dx%d\n", width, height);
//  init();
//  redraw();
}

void GUI::NativeWindowPugl::move(int x, int y)
{
//  redraw();
}

void GUI::NativeWindowPugl::show()
{
//  redraw();
}

void GUI::NativeWindowPugl::hide()
{
//  redraw();
}

void GUI::NativeWindowPugl::handleBuffer()
{
  onDisplay(view);
}

void GUI::NativeWindowPugl::redraw()
{
//  handleBuffer();
}

void GUI::NativeWindowPugl::setCaption(const std::string &caption)
{
//  redraw();
}

void GUI::NativeWindowPugl::grabMouse(bool grab)
{
//  redraw();
}

bool GUI::NativeWindowPugl::hasEvent()
{
  // dirty hack - assume that this function is called enough times to get fluent gui
  // ^^ Bad assumption
  puglProcessEvents(view);
  return !eventq.empty();
}

GUI::Event *GUI::NativeWindowPugl::getNextEvent()
{
  Event *event = NULL;
  
  if(!eventq.empty()) {
    event = eventq.front();
    eventq.pop_front();
  }
  return event;
}

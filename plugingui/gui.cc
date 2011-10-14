/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            gui.cc
 *
 *  Mon Oct  3 13:40:49 CEST 2011
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
#include "gui.h"

#include <stdio.h>

#include "globalcontext.h"

#include "button.h"
#include "lineedit.h"

GUI::GUI()
{
  gctx = new GlobalContext();
  eventhandler = new EventHandler(gctx);
  window = new _Window(gctx);
  window->setSize(450, 40);

  LineEdit *l = new LineEdit(gctx, window);
  l->setText("");
  l->move(10, 10);
  l->setSize(210, 20);

  Button *b1 = new Button(gctx, window);
  b1->setText("OK");
  b1->move(230, 10);
  b1->setSize(100, 20);

  Button *b2 = new Button(gctx, window);
  b2->setText("Cancel");
  b2->move(340, 10);
  b2->setSize(100, 20);
}

GUI::~GUI()
{
  delete window;
  delete eventhandler;
  delete gctx;
}

void GUI::show()
{
  window->show();
}

void GUI::hide()
{
  window->hide();
}

void GUI::processEvents()
{
  while(eventhandler->hasEvent()) {
    Event *event = eventhandler->getNextEvent();

    if(event == NULL) continue;

    Widget *widget = gctx->widgets[event->window_id];
    switch(event->type()) {
    case Event::Repaint:
      widget->repaint((RepaintEvent*)event);
      break;
    case Event::MouseMove:
      widget->mouseMove((MouseMoveEvent*)event);
      break;
    case Event::Button:
      widget->button((ButtonEvent*)event);
      break;
    case Event::Key:
      //widget->key((KeyEvent*)event);
      if(gctx->keyboardFocus) gctx->keyboardFocus->key((KeyEvent*)event);
      break;
    }

    delete event;
  }
}

#ifdef STANDALONE

int main()
{
  GUI gui;

  gui.show();

  while(true) {
    gui.processEvents();
    usleep(1000);
  }

  return 0;
}

#endif/*STANDALONE*/

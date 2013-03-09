/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            listbox.cc
 *
 *  Mon Feb 25 21:21:41 CET 2013
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
#include "listbox.h"

#include "painter.h"
#include "font.h"

#include <stdio.h>

GUI::ListBox::ListBox(GUI::Widget *parent)
  : GUI::Widget(parent)
{
  padding = 4;
  btn_size = 14;

  scroll_offset = 0;
  selected = "";

  dblclk_handler = NULL;
  ptr = NULL;
}

GUI::ListBox::~ListBox()
{
  // printf("ListBox destroy\n");
}

void GUI::ListBox::addItem(std::string name, std::string value)
{
  items[name] = value;
  if(selected == "") selected = value;
}

void GUI::ListBox::clear()
{
  items.clear();
  selected = "";
  scroll_offset = 0;
  repaintEvent(NULL);
}

bool GUI::ListBox::selectItem(std::string name)
{
  if(items.find(name) == items.end()) return false;
  selected = items[name];
  repaintEvent(NULL);
  return true;
}

std::string GUI::ListBox::selectedName()
{
  return selected;
}

std::string GUI::ListBox::selectedValue()
{
  return items[selected];
}

void GUI::ListBox::registerDblClickHandler(void (*handler)(void *), void *ptr)
{
  this->dblclk_handler = handler;
  this->ptr = ptr;
}

void GUI::ListBox::repaintEvent(GUI::RepaintEvent *e)
{
  GUI::Painter p(this);

  p.setColour(Colour(0, 0.5));
  p.drawFilledRectangle(0, 0, width() - 1, height() - 1);

  p.setColour(Colour(0.5, 1));
  p.drawRectangle(0, 0, width() - 1, height() - 1);

  int yoffset = padding / 2;
  int skip = scroll_offset;
  std::map<std::string, std::string>::iterator i = items.begin();
  while(i != items.end()) {
    if(skip) {
      skip--;
      i++;
      continue;
    }
    if(i->second == selected) {
      p.setColour(Colour(1, 0.4));
      p.drawFilledRectangle(1,
                            yoffset - (padding / 2),
                            width() - 1,
                            yoffset + (font.textHeight() + 1));
    }

    p.setColour(Colour(1, 1));
    p.drawText(2, yoffset + font.textHeight(), font, i->first);
    yoffset += font.textHeight() + padding;
    i++;
  }

  p.drawRectangle(width() - btn_size, 0, width() - 1, btn_size);
  p.drawRectangle(width() - btn_size, height() - btn_size,
                  width() - 1, height() - 1);
}

void GUI::ListBox::scrollEvent(ScrollEvent *e)
{
  scroll_offset += e->delta;
  if(scroll_offset < 0) scroll_offset = 0;
  if(scroll_offset > (items.size() - 1))
    scroll_offset = (items.size() - 1);
  repaintEvent(NULL);
}

void GUI::ListBox::keyEvent(GUI::KeyEvent *e)
{
  //  printf("!\n");

  if(e->direction == -1) {
    switch(e->keycode) {
    case GUI::KeyEvent::KEY_UP:
      {
        std::map<std::string, std::string>::reverse_iterator i = items.rbegin();
        while(i != items.rend()) {
          if(i->second == selected) break;
          i++;
        }
        i++;
        scroll_offset--;
        if(scroll_offset < 0) scroll_offset = 0;
        selected = i->second;
      }
      break;
    case GUI::KeyEvent::KEY_DOWN:
      {
        std::map<std::string, std::string>::iterator i = items.begin();
        while(i != items.end()) {
          if(i->second == selected) break;
          i++;
        }
        i++;
        scroll_offset++;
        if(scroll_offset > (items.size() - 1))
          scroll_offset = (items.size() - 1);
        selected = i->second;
      }
      break;
    case GUI::KeyEvent::KEY_HOME:
      selected = items.begin()->second;
      break;
    case GUI::KeyEvent::KEY_END:
      selected = items.rbegin()->second;
      break;
    default:
      break;
    }

    //  printf("sel: %s\n", selected.c_str());

    repaintEvent(NULL);
  }

}

void GUI::ListBox::buttonEvent(ButtonEvent *e)
{
  if(e->direction == 1) {
    if(e->x > (width() - btn_size) && e->y < (width() - 1)) {
      if(e->y > 0 && e->y < btn_size) {
        scroll_offset--;
        if(scroll_offset < 0) scroll_offset = 0;
        repaintEvent(NULL);
        return;
      }

      if(e->y > (height() - btn_size) && e->y < (height() - 1)) {
        scroll_offset++;
        if(scroll_offset > (items.size() - 1))
          scroll_offset = (items.size() - 1);
        repaintEvent(NULL);
        return;
      }
    }

    int skip = scroll_offset;
    size_t yoffset = padding / 2;
    std::map<std::string, std::string>::iterator i = items.begin();
    while(i != items.end()) {
      if(skip) {
        skip--;
        i++;
        continue;
      }
      yoffset += font.textHeight() + padding;
      if(e->y < (int)yoffset - (padding / 2)) {
        selected = i->second;
        break;
      }
      i++;
    }

    repaintEvent(NULL);
  }

  if(e->doubleclick && dblclk_handler) dblclk_handler(ptr);
}

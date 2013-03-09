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
  selected = -1;

  dblclk_handler = NULL;
  ptr = NULL;
}

GUI::ListBox::~ListBox()
{
}

void GUI::ListBox::addItem(std::string name, std::string value)
{
  struct item i;
  i.name = name;
  i.value = value;
 
  items.push_back(i);

  // sort
  for(int x = 0; x < (int)items.size() - 1; x++) {
    for(int y = 0; y < (int)items.size() - 1; y++) {
      if(items[x].name < items[y].name) {

        if(x == selected) selected = y;
        else if(selected == y) selected = x;

        struct item tmp = items[x];
        items[x] = items[y];
        items[y] = tmp;
      }
    }
  }

  if(selected == -1) selected = items.size() - 1;
}

void GUI::ListBox::clear()
{
  items.clear();
  selected = -1;
  scroll_offset = 0;
  repaintEvent(NULL);
}

bool GUI::ListBox::selectItem(int index)
{
  if(index < 0 || index > (int)items.size() - 1) return false;
  selected = index;
  repaintEvent(NULL);
  return true;
}

std::string GUI::ListBox::selectedName()
{
  if(selected < 0 || selected > (int)items.size() - 1) return "";
  return items[selected].name;
}

std::string GUI::ListBox::selectedValue()
{
  if(selected < 0 || selected > (int)items.size() - 1) return "";
  return items[selected].value;
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
  for(int idx = skip; idx < (int)items.size(); idx++) {
    struct item *i = &items[idx];
    if(idx == selected) {
      p.setColour(Colour(1, 0.4));
      p.drawFilledRectangle(1,
                            yoffset - (padding / 2),
                            width() - 1,
                            yoffset + (font.textHeight() + 1));
    }

    p.setColour(Colour(1, 1));
    p.drawText(2, yoffset + font.textHeight(), font, i->name);
    yoffset += font.textHeight() + padding;
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
  if(e->direction != -1) return;

  switch(e->keycode) {
  case GUI::KeyEvent::KEY_UP:
    {
      selected--;
      if(selected < 0) selected = 0;
      if(selected < scroll_offset) {
        scroll_offset = selected;
        if(scroll_offset < 0) scroll_offset = 0;
      }
    }
    break;
  case GUI::KeyEvent::KEY_DOWN:
    {
      // Number of items that can be displayed at a time.
      int numitems = height() / (font.textHeight() + padding);

      selected++;
      if(selected > (items.size() - 1))
        selected = (items.size() - 1);
      if(selected > (scroll_offset + numitems - 1)) {
        scroll_offset = selected - numitems + 1;
        if(scroll_offset > (items.size() - 1))
          scroll_offset = (items.size() - 1);
      }
    }
    break;
  case GUI::KeyEvent::KEY_HOME:
    selected = 0;
    break;
  case GUI::KeyEvent::KEY_END:
    selected = items.size() - 1;
    break;
  default:
    break;
  }
  
  repaintEvent(NULL);
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
    for(int idx = skip; idx < (int)items.size() - 1; idx++) {
      yoffset += font.textHeight() + padding;
      if(e->y < (int)yoffset - (padding / 2)) {
        selected = idx;
        break;
      }
    }

    repaintEvent(NULL);
  }

  if(e->doubleclick && dblclk_handler) dblclk_handler(ptr);
}

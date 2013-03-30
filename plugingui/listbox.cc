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
  box.topLeft     = new Image(":widget_tl.png");
  box.top         = new Image(":widget_t.png");
  box.topRight    = new Image(":widget_tr.png");
  box.left        = new Image(":widget_l.png");
  box.right       = new Image(":widget_r.png");
  box.bottomLeft  = new Image(":widget_bl.png");
  box.bottom      = new Image(":widget_b.png");
  box.bottomRight = new Image(":widget_br.png");
  box.center      = new Image(":widget_c.png");

  padding = 4;
  btn_size = 14;

  scroll_offset = 0;
  selected = -1;
  marked = -1;

  clk_handler = NULL;
  clk_ptr = NULL;

  sel_handler = NULL;
  sel_ptr = NULL;

  valch_handler = NULL;
  valch_ptr = NULL;
}

GUI::ListBox::~ListBox()
{
}

void GUI::ListBox::setSelection(int index)
{
  selected = index;
  if(valch_handler) valch_handler(valch_ptr);
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

        if(x == selected) setSelection(y);
        else if(selected == y) setSelection(x);

        struct item tmp = items[x];
        items[x] = items[y];
        items[y] = tmp;
      }
    }
  }

  if(selected == -1) setSelection((int)items.size() - 1);
}

void GUI::ListBox::clear()
{
  items.clear();
  setSelection(-1);
  scroll_offset = 0;
  repaintEvent(NULL);
}

bool GUI::ListBox::selectItem(int index)
{
  if(index < 0 || index > (int)items.size() - 1) return false;
  setSelection(index);
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

void GUI::ListBox::registerClickHandler(void (*handler)(void *), void *ptr)
{
  this->clk_handler = handler;
  this->clk_ptr = ptr;
}

void GUI::ListBox::registerSelectHandler(void (*handler)(void *), void *ptr)
{
  this->sel_handler = handler;
  this->sel_ptr = ptr;
}

void GUI::ListBox::registerValueChangeHandler(void (*handler)(void *),
                                              void *ptr)
{
  this->valch_handler = handler;
  this->valch_ptr = ptr;
}

void GUI::ListBox::repaintEvent(GUI::RepaintEvent *e)
{
  GUI::Painter p(this);

  p.clear();

  int w = width();
  int h = height();
  if(w == 0 || h == 0) return;
  p.drawBox(0, 0, &box, w, h);

  int yoffset = padding / 2;
  int skip = scroll_offset;
  for(int idx = skip; idx < (int)items.size(); idx++) {
    struct item *i = &items[idx];
    if(idx == selected) {
      p.setColour(Colour(0.6, 0.9));
      p.drawFilledRectangle(1,
                            yoffset - (padding / 2),
                            width() - 1,
                            yoffset + (font.textHeight() + 1));
    }

    if(idx == marked) {
      p.setColour(Colour(1, 0.9));
      p.drawRectangle(1,
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
  if(scroll_offset > ((int)items.size() - 1))
    scroll_offset = ((int)items.size() - 1);
  repaintEvent(NULL);
}

void GUI::ListBox::keyEvent(GUI::KeyEvent *e)
{
  if(e->direction != -1) return;

  switch(e->keycode) {
  case GUI::KeyEvent::KEY_UP:
    {
      marked--;
      if(marked < 0) marked = 0;

      if(marked < scroll_offset) {
        scroll_offset = marked;
        if(scroll_offset < 0) scroll_offset = 0;
      }
    }
    break;
  case GUI::KeyEvent::KEY_DOWN:
    {
      // Number of items that can be displayed at a time.
      int numitems = height() / (font.textHeight() + padding);

      marked++;
      if(marked > ((int)items.size() - 1)) marked = (int)items.size() - 1;

      if(marked > (scroll_offset + numitems - 1)) {
        scroll_offset = marked - numitems + 1;
        if(scroll_offset > ((int)items.size() - 1))
          scroll_offset = ((int)items.size() - 1);
      }
    }
    break;
  case GUI::KeyEvent::KEY_HOME:
    marked = 0;
    if(marked < scroll_offset) {
      scroll_offset = marked;
      if(scroll_offset < 0) scroll_offset = 0;
    }
    break;
  case GUI::KeyEvent::KEY_END:
    {
      // Number of items that can be displayed at a time.
      int numitems = height() / (font.textHeight() + padding);

      marked = (int)items.size() - 1;
      if(marked > (scroll_offset + numitems - 1)) {
        scroll_offset = marked - numitems + 1;
        if(scroll_offset > ((int)items.size() - 1))
          scroll_offset = ((int)items.size() - 1);
      }
    }
    break;
  case GUI::KeyEvent::KEY_CHARACTER:
    if(e->text == " ") {
      setSelection(marked);
      // if(sel_handler) sel_handler(sel_ptr);
    }
    break;
  case GUI::KeyEvent::KEY_ENTER:
    setSelection(marked);
    if(sel_handler) sel_handler(sel_ptr);
    break;
  default:
    break;
  }
  
  repaintEvent(NULL);
}

void GUI::ListBox::buttonEvent(ButtonEvent *e)
{
  if(e->x > ((int)width() - btn_size) && e->y < ((int)width() - 1)) {
    if(e->y > 0 && e->y < btn_size) {
      if(e->direction == -1) return;
      scroll_offset--;
      if(scroll_offset < 0) scroll_offset = 0;
      repaintEvent(NULL);
      return;
    }
    
    if(e->y > ((int)height() - btn_size) && e->y < ((int)height() - 1)) {
      if(e->direction == -1) return;
      scroll_offset++;
      if(scroll_offset > ((int)items.size() - 1))
        scroll_offset = ((int)items.size() - 1);
      repaintEvent(NULL);
      return;
    }
  }

  if(e->direction == -1) {
    int skip = scroll_offset;
    size_t yoffset = padding / 2;
    for(int idx = skip; idx < (int)items.size(); idx++) {
      yoffset += font.textHeight() + padding;
      if(e->y < (int)yoffset - (padding / 2)) {
        setSelection(idx);
        marked = selected;
        if(clk_handler) clk_handler(clk_ptr);
        break;
      }
    }

    repaintEvent(NULL);
  }

  if(e->direction != -1) {
    int skip = scroll_offset;
    size_t yoffset = padding / 2;
    for(int idx = skip; idx < (int)items.size(); idx++) {
      yoffset += font.textHeight() + padding;
      if(e->y < (int)yoffset - (padding / 2)) {
        marked = idx;
        break;
      }
    }

    repaintEvent(NULL);
  }

  if(e->doubleclick && sel_handler) sel_handler(sel_ptr);
}

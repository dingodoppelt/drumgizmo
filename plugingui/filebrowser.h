/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            filebrowser.h
 *
 *  Mon Feb 25 21:09:43 CET 2013
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
#ifndef __DRUMGIZMO_FILEBROWSER_H__
#define __DRUMGIZMO_FILEBROWSER_H__

#include "widget.h"

#include "button.h"
#include "listbox.h"

namespace GUI {

class FileBrowser : public Widget {
public:
  struct private_data;

  FileBrowser(Widget *parent);
  ~FileBrowser();

  bool isFocusable() { return true; }

  void registerFileSelectHandler(void (*handler)(void *, std::string),
                                 void *ptr);

  virtual void repaintEvent(RepaintEvent *e);

  virtual void resize(size_t w, size_t h);

private:
  struct private_data *prv;

  GUI::ListBox *listbox;
  GUI::Button *btn;
};

};

#endif/*__DRUMGIZMO_FILEBROWSER_H__*/
/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            filebrowser.cc
 *
 *  Mon Feb 25 21:09:44 CET 2013
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
#include "filebrowser.h"

#include "painter.h"

#include "button.h"
#include "listbox.h"

#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct GUI::FileBrowser::private_data {
  GUI::ListBox *listbox;
  void (*filesel_handler)(void *, std::string);
  void *ptr;
};

void changeDir(void *ptr)
{
  struct GUI::FileBrowser::private_data *prv =
    (struct GUI::FileBrowser::private_data *)ptr;

  GUI::ListBox *lb = prv->listbox;
  std::string value = lb->selectedValue();

  char filename[1024];
  char *c = getcwd(filename, sizeof(filename));
  (void)c;
  strcat(filename, "/");
  strcat(filename, value.c_str());

  struct stat st;
  if(stat(filename, &st) == 0) {
    if((st.st_mode & S_IFDIR) != 0) {
      //printf("'%s' is present and is a directory\n", filename);
    }
    if((st.st_mode & S_IFREG) != 0) {
      //printf("'%s' is present and is a file\n", filename);
      if(prv->filesel_handler) prv->filesel_handler(prv->ptr, filename);
      return;
    }
  } else {
    //printf("'%s' is not present or unreadable\n", filename);
    return;
  }

  lb->clear();
  int i = chdir(value.c_str());
  (void)i;
  DIR *dir = opendir(".");

  struct dirent *entry;
  while((entry = readdir(dir)) != NULL) {
    lb->addItem(entry->d_name, entry->d_name);
  }

  closedir(dir);
}

GUI::FileBrowser::FileBrowser(GUI::Widget *parent)
  : GUI::Widget(parent)
{
  prv = new struct GUI::FileBrowser::private_data();
  prv->filesel_handler = NULL;

#define brd 5 // border
#define btn_h 12

  listbox = new GUI::ListBox(this);
  listbox->registerDblClickHandler(changeDir, prv);
  prv->listbox = listbox;

  btn = new GUI::Button(this);
  btn->setText("Select");
  btn->registerClickHandler(changeDir, prv);

  changeDir(prv);

  resize(200, 190);
}

GUI::FileBrowser::~FileBrowser()
{
  delete prv->listbox;
  delete prv;
}

void GUI::FileBrowser::resize(size_t w, size_t h)
{
  GUI::Widget::resize(w,h);

  listbox->move(brd, brd);
  listbox->resize(w - 1 - 2*brd, h - btn_h -  3*brd);

  btn->move(brd, h - btn_h - brd);
  btn->resize(w - 1 - 2*brd, btn_h);
}


void GUI::FileBrowser::registerFileSelectHandler(void (*handler)(void *,
                                                        std::string),
                                                 void *ptr)
{
  prv->filesel_handler = handler;
  prv->ptr = ptr;
}

void GUI::FileBrowser::repaintEvent(GUI::RepaintEvent *e)
{
  Painter p(this);
  p.setColour(Colour(0, 0.8));

  p.drawFilledRectangle(0, 0, width(), height());

  p.setColour(Colour(1, 1));

  for(int i = 1; i < 10; i++) {
    p.drawLine(0,0,width() / i, height() - 1);
    p.drawLine(width()-1,0,width() / i, height() - 1);
  }
}

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

#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <hugin.hpp>

#ifdef WIN32
#include <direct.h>
#endif

struct GUI::FileBrowser::private_data {
  GUI::LineEdit *lineedit;
  GUI::ListBox *listbox;
  GUI::ComboBox *drives;
  void (*filesel_handler)(void *, std::string);
  void *ptr;
#ifdef WIN32
  int drvidx;
#endif
};

static void cancel(void *ptr)
{
  GUI::FileBrowser *fp = (GUI::FileBrowser *)ptr;
  fp->hide();
}

static void changeDir(void *ptr)
{
  struct GUI::FileBrowser::private_data *prv =
    (struct GUI::FileBrowser::private_data *)ptr;

  GUI::ListBox *lb = prv->listbox;
  GUI::LineEdit *le = prv->lineedit;
  std::string value = lb->selectedValue();

#ifdef WIN32
    std::string drive = prv->drives->selectedValue();
    int drvidx = atoi(drive.c_str());
    /*if(prv->drvidx != drvidx)*/ _chdrive(drvidx + 1); // one based... sigh
    //printf("DRV: [%d %s]\n", drvidx, drive.c_str());
#endif

  char filename[1024];
  char *c = getcwd(filename, sizeof(filename));
  (void)c;

  //printf("CWD: [%s]\n", filename);

  if(value != "") {
#ifdef WIN32
    if(prv->drvidx == drvidx) {
      strcat(filename, "\\");
      strcat(filename, value.c_str());
    }
    prv->drvidx = drvidx;
#else
    strcat(filename, "/");
    strcat(filename, value.c_str());
#endif
  }

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
    //perror("!");
    return;
  }

  lb->clear();
  int i = chdir(value.c_str());
  (void)i;

  c = getcwd(filename, sizeof(filename));
  le->setText(filename);

  DIR *dir = opendir(".");
  if(!dir) {
    lb->addItem("[ Could not open dir ]", "");
    return;
  }

 struct dirent *entry;
  while((entry = readdir(dir)) != NULL) {
    lb->addItem(entry->d_name, entry->d_name);
  }

  closedir(dir);
}

GUI::FileBrowser::FileBrowser(GUI::Widget *parent)
  : GUI::Widget(parent),
    lbl_path(this), lineedit(this), listbox(this), btn_sel(this), btn_esc(this)
#ifdef WIN32
  , drv(this), lbl_drive(this)
#endif   
{
  prv = new struct GUI::FileBrowser::private_data();
  prv->filesel_handler = NULL;

  lbl_path.setText("Path:");

  lineedit.setReadOnly(true);
  prv->lineedit = &lineedit;

  prv->listbox = &listbox;
  listbox.registerSelectHandler(changeDir, prv);

  btn_sel.setText("Select");
  btn_sel.registerClickHandler(changeDir, prv);

  btn_esc.setText("Cancel");
  btn_esc.registerClickHandler(cancel, this);

#ifdef WIN32
  lbl_drive.setText("Drive:");

  drv.registerValueChangedHandler(changeDir, prv);

  unsigned int d = GetLogicalDrives();
  for(int i = 0; i < 32; i++) {
    if(d & (1 << i)) {
      
      char name[] = "X:";
      name[0] = i + 'A';

      char num[32];
      sprintf(num, "%d", i);
      
      drv.addItem(name, num);
    }
  }
  prv->drives = &drv;
#endif

  changeDir(prv);

  resize(200, 190);
}

GUI::FileBrowser::~FileBrowser()
{
  //  delete prv->listbox;
  delete prv;
}

void GUI::FileBrowser::setPath(std::string path)
{
  std::string dirname = path;

  while(dirname != "") {

    DEBUG(filebrowser, "dirname: %s\n", dirname.c_str());

    struct stat st;
    if(stat(dirname.c_str(), &st) == 0) {
      if((st.st_mode & S_IFDIR) != 0) {
        dirname += "/.";
        int i = chdir(dirname.c_str());
        (void)i;
        changeDir(prv);

        DEBUG(filebrowser, "chdir to: %s\n", dirname.c_str());

        return;
      }
    }

    dirname = dirname.substr(0, dirname.length() - 1);
    while(dirname[dirname.length() - 1] != '/' &&
          dirname[dirname.length() - 1] != '\\' &&
          dirname != "") { 
      dirname = dirname.substr(0, dirname.length() - 1);
    }
  }
}

void GUI::FileBrowser::resize(size_t w, size_t h)
{
  GUI::Widget::resize(w,h);

  int offset = 0;
  int brd = 5; // border
  int btn_h = 18;

#ifdef WIN32
  offset += brd;

  lbl_drive.move(0, offset);
  drv.move(60, offset);

  offset += btn_h;

  lbl_drive.resize(60, btn_h);
  drv.resize(w - 60 - brd, btn_h);
#endif
  offset += brd;

  lbl_path.move(0, offset);
  lineedit.move(60, offset);

  offset += btn_h;

  lbl_path.resize(60, btn_h);
  lineedit.resize(w - 60 - brd, btn_h);

  offset += brd;

  listbox.move(brd, offset);
  listbox.resize(w - 1 - 2*brd, h - btn_h - 2*brd - offset);

  btn_esc.move(brd, h - btn_h - brd);
  btn_esc.resize((w - 1 - 2*brd) / 2 - brd / 2, btn_h);

  btn_sel.move(brd + w / 2 - brd / 2, h - btn_h - brd);
  btn_sel.resize((w - 1 - 2*brd) / 2, btn_h);


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

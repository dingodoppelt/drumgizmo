/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            plugingui.h
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
#ifndef __DRUMGIZMO_PLUGINGUI_H__
#define __DRUMGIZMO_PLUGINGUI_H__

#include "window.h"
#include "eventhandler.h"
#include "globalcontext.h"

#include "label.h"
#include "filelineedit.h"
#include "checkbox.h"
#include "button.h"
#include "knob.h"

#include "filebrowser.h"

#include "thread.h"

class DrumGizmo;

class PluginGUI : public Thread {
public:
  PluginGUI(DrumGizmo *drumgizmo);
  ~PluginGUI();

  void thread_main();

  void init();

  void show();
  void hide();
  void processEvents();
  void setWindowClosedCallback(void (*handler)(void *), void *ptr);

  void setChangeMidimapCallback(void (*handler)(void *, const char *),
                                void *ptr);

  //private:
  GUI::GlobalContext *gctx;
  GUI::Window *window;
  GUI::EventHandler *eventhandler;

  GUI::FileBrowser *filebrowser;

  DrumGizmo *drumgizmo;

  GUI::CheckBox *check;
  GUI::Knob *knob;
  GUI::Knob *knob2;

  GUI::Button *btn_ok;
  GUI::Button *btn_ok2;
  GUI::Button *btn_cancel;

  GUI::Label *lbl;
  FileLineEdit *lineedit;
  GUI::LED *led;

  GUI::Label *lbl2;
  FileLineEdit *lineedit2;
  GUI::LED *led2;

  void (*windowClosedHandler)(void *);
  void *windowClosedPtr;

  void (*changeMidimapHandler)(void *, const char *);
  void *changeMidimapPtr;

private:
  volatile bool running;
};

#endif/*__DRUMGIZMO_PLUGINGUI_H__*/

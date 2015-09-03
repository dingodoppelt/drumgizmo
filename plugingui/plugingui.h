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
#pragma once

#include "window.h"
#include "eventhandler.h"

#include "label.h"
#include "lineedit.h"
#include "checkbox.h"
#include "button.h"
#include "knob.h"
#include "progressbar.h"
#include "pluginconfig.h"

#include "filebrowser.h"

#include "thread.h"
#include "semaphore.h"

#include "messagereceiver.h"
#include "notifier.h"

namespace GUI {

class PluginGUI : public Thread, public MessageReceiver, public Listener {
public:
  PluginGUI();
  virtual ~PluginGUI();

  void thread_main();
  void stopThread();

  void init();
  void deinit();

  void show();
  void hide();
  void processEvents();
  void setWindowClosedCallback(void (*handler)(void *), void *ptr);

  void handleMessage(Message *msg);

  Window *window;
  EventHandler *eventhandler;

  FileBrowser *filebrowser;

  Label *lbl;
  LineEdit *lineedit;
  ProgressBar *progress;

  Label *lbl2;
  LineEdit *lineedit2;
  ProgressBar *progress2;

  Config *config;

  void (*windowClosedHandler)(void *);
  void *windowClosedPtr;

  void (*changeMidimapHandler)(void *, const char *);
  void *changeMidimapPtr;

private:
  void attackValueChanged(float value);
  void falloffValueChanged(float value);
  void velocityCheckClick(bool checked);

  // Humanized velocity controls:
  CheckBox *velocityCheck;
  Knob *attackKnob;
  Knob *falloffKnob;

  volatile bool running;
  volatile bool closing;
  volatile bool initialised;

  Semaphore sem;
};

} // GUI::


/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            plugingui.cc
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
#include "plugingui.h"

#include <hugin.hpp>
#include <stdio.h>

#include "globalcontext.h"
#include "knob.h"

#include "verticalline.h"

#ifndef STANDALONE
#include <drumgizmo.h>
#include "../src/configuration.h"
#else
#include "../src/message.h"
class DrumGizmo {
public:
  bool loadkit(std::string) { return true; }
  bool init(bool) { return true; }
  std::string drumkitfile() { return ""; }
  std::string midimapfile;
  Message *receiveGUIMessage() { return NULL; }
  Message *peekGUIMessage() { return NULL; }
  void sendEngineMessage(Message *msg) { delete msg; }
};

namespace Conf {
  bool enable_velocity_modifier;
  float velocity_modifier_weight;
  float velocity_modifier_falloff;
};
#endif

static void checkClick(void *ptr)
{
  PluginGUI *gui = (PluginGUI*)ptr;
  Conf::enable_velocity_modifier = gui->check->checked();
}

static void knobChange(void *ptr)
{
  PluginGUI *gui = (PluginGUI*)ptr;
  Conf::velocity_modifier_weight = gui->knob->value();
#ifdef STANDALONE
  int i = gui->knob->value() * 4;
  switch(i) {
  case 0: gui->progress->setState(GUI::ProgressBar::off); break;
  case 1: gui->progress->setState(GUI::ProgressBar::blue); break;
  case 2: gui->progress->setState(GUI::ProgressBar::green); break;
  case 3: gui->progress->setState(GUI::ProgressBar::red); break;
  default: break;
  }
#endif
}

static void knobChange2(void *ptr)
{
  PluginGUI *gui = (PluginGUI*)ptr;
  Conf::velocity_modifier_falloff = gui->knob2->value();
#ifdef STANDALONE
  gui->progress->setProgress(gui->knob2->value());
#endif
}

GUI::FileBrowser *fb;
static void selectKitFile(void *ptr, std::string filename)
{
  PluginGUI *gui = (PluginGUI*)ptr;

  gui->lineedit->setText(filename);
  fb->hide();

  std::string drumkit = gui->lineedit->text();

  gui->progress->setProgress(0);
  gui->progress->setState(GUI::ProgressBar::blue);

  LoadDrumKitMessage *msg = new LoadDrumKitMessage();
  msg->drumkitfile = drumkit;
  gui->drumgizmo->sendEngineMessage(msg);
  /*
  if(!gui->drumgizmo ||
     !gui->drumgizmo->loadkit(drumkit) ||
     !gui->drumgizmo->init(true)) {
    gui->progress->setState(GUI::ProgressBar::red);
  } else {
    gui->progress->setState(GUI::ProgressBar::blue);
  }
  */
}

static void kitBrowseClick(void *ptr)
{
  PluginGUI *gui = (PluginGUI*)ptr;

  fb->setPath(gui->lineedit->text());
  fb->registerFileSelectHandler(selectKitFile, gui);
  fb->show();
}

static void selectMapFile(void *ptr, std::string filename)
{
  PluginGUI *gui = (PluginGUI*)ptr;

  gui->lineedit2->setText(filename);
  fb->hide();

  std::string midimap = gui->lineedit2->text();

  LoadMidimapMessage *msg = new LoadMidimapMessage();
  msg->midimapfile = midimap;
  gui->drumgizmo->sendEngineMessage(msg);

  /*
  if(gui->changeMidimapHandler)
    gui->changeMidimapHandler(gui->changeMidimapPtr, midimap.c_str());
  gui->progress2->setState(GUI::ProgressBar::green);
  */
}

static void midimapBrowseClick(void *ptr)
{
  PluginGUI *gui = (PluginGUI*)ptr;

  fb->setPath(gui->lineedit2->text());
  fb->registerFileSelectHandler(selectMapFile, gui);
  fb->show();
}

/*
void closeClick(void *ptr)
{
  PluginGUI *gui = (PluginGUI*)ptr;
  if(gui->windowClosedHandler) gui->windowClosedHandler(gui->windowClosedPtr);
}
*/

#include "../version.h"

PluginGUI::PluginGUI(DrumGizmo *drumgizmo)
{
  windowClosedHandler = NULL;
  changeMidimapHandler = NULL;

  this->drumgizmo = drumgizmo;

  gctx = NULL;
  window = NULL;
  eventhandler = NULL;
 
  running = true;

#ifdef USE_THREAD
  run();
#else
  init();
#endif/*USE_THREAD*/

  sem.wait();
}

PluginGUI::~PluginGUI()
{
  DEBUG(plugingui, "~PluginGUI()\n");

  running = false;
  wait_stop();

  if(window) delete window;
  if(eventhandler) delete eventhandler;
  if(gctx) delete gctx;
}

void PluginGUI::thread_main()
{
  init();

  while(running) {
    eventhandler->processEvents(window);
#ifdef WIN32
    SleepEx(50, FALSE);
#else
    usleep(50);
#endif/*WIN32*/
    Message *msg;
    while((msg = drumgizmo->receiveGUIMessage()) != NULL) {
      switch(msg->type()) {
      case Message::LoadStatus:
        {
          Message *pmsg;
          while( (pmsg = drumgizmo->peekGUIMessage()) != NULL) {
            if(pmsg->type() != Message::LoadStatus) break;
            delete msg;
            msg = drumgizmo->receiveGUIMessage();
          } 
          LoadStatusMessage *ls = (LoadStatusMessage*)msg;
          DEBUG(gui, "%d of %d (%s)\n",
                ls->numer_of_files_loaded,
                ls->number_of_files,
                ls->current_file.c_str());
          progress->setProgress((float)ls->numer_of_files_loaded /
                                (float)ls->number_of_files);
          if(ls->numer_of_files_loaded == ls->number_of_files) {
            progress->setState(GUI::ProgressBar::green);
          }
        }
        break;
      default:
        break;
      }

      delete msg;
    }
  }
}

void PluginGUI::init()
{
  DEBUG(gui, "init");
  gctx = new GUI::GlobalContext();
  eventhandler = new GUI::EventHandler(gctx);
  //  printf("%p\n", eventhandler);
  window = new GUI::Window(gctx);
  window->resize(370, 330);
  window->setCaption("DrumGizmo v"VERSION);

  GUI::Label *lbl_title = new GUI::Label(window);
  lbl_title->setText("DrumGizmo");
  lbl_title->move(127, 7);
  lbl_title->resize(200, 20);
  
  GUI::VerticalLine *l1 = new GUI::VerticalLine(window);
  l1->move(20, 30);
  l1->resize(window->width() - 40, 2);

#define OFFSET1 17
#define OFFSET2 38
#define OFFSET3 20

#define XOFFSET 20
  // Drumkit file
  {
    int y = 37;
    GUI::Label *lbl = new GUI::Label(window);
    lbl->setText("Drumkit file:");
    lbl->move(XOFFSET - 4, y);
    lbl->resize(100, 20);

    y += OFFSET1;
    lineedit = new GUI::LineEdit(window);
    if(drumgizmo) lineedit->setText(drumgizmo->drumkitfile());
    else lineedit->setText("/home/deva/aasimonster/aasimonster.xml");
    //else lineedit->setText("Missing DrumGizmo*");
    lineedit->move(XOFFSET, y);
    lineedit->resize(243, 29);
    
    GUI::Button *btn_brw = new GUI::Button(window);
    btn_brw->setText("Browse...");
    btn_brw->move(266, y - 6 + 4);
    btn_brw->resize(85, 35 + 6 - 4);
    btn_brw->registerClickHandler(kitBrowseClick, this);

    y += OFFSET2;
    progress = new GUI::ProgressBar(window);
    progress->move(XOFFSET, y);
    progress->resize(window->width() - 2*XOFFSET, 11);

    y += OFFSET3;
    GUI::VerticalLine *l = new GUI::VerticalLine(window);
    l->move(XOFFSET, y);
    l->resize(window->width() - 2*XOFFSET, 2);
  }

  // Midimap file
  {
    int y = 120;
    lbl2 = new GUI::Label(window);
    lbl2->setText("Midimap file:");
    lbl2->move(XOFFSET - 4, y);
    lbl2->resize(100, 20);
    
    y += OFFSET1;
    lineedit2 = new GUI::LineEdit(window);
    if(drumgizmo) lineedit2->setText(drumgizmo->midimapfile);
    lineedit2->move(XOFFSET, y);
    lineedit2->resize(243, 29);
    
    GUI::Button *btn_brw = new GUI::Button(window);
    btn_brw->setText("Browse...");
    btn_brw->move(266, y - 6 + 4);
    btn_brw->resize(85, 35 + 6 - 4);
    btn_brw->registerClickHandler(midimapBrowseClick, this);

    y += OFFSET2;
    progress2 = new GUI::ProgressBar(window);
    progress2->move(XOFFSET, y);
    progress2->resize(window->width() - 2*XOFFSET, 11);

    y += OFFSET3;
    GUI::VerticalLine *l = new GUI::VerticalLine(window);
    l->move(XOFFSET, y);
    l->resize(window->width() - 2*XOFFSET, 2);
  }

  {
    int y = 203;
#define OFFSET4 21

    // Enable Velocity
    GUI::Label *lbl_velocity = new GUI::Label(window);
    lbl_velocity->resize(78 ,20);
    lbl_velocity->move(16, y);
    lbl_velocity->setText("Humanizer");

    check = new GUI::CheckBox(window);
    //check->setText("Enable Velocity Modifier");
    check->move(26, y + OFFSET4);
    check->resize(59,38);
    check->setChecked(Conf::enable_velocity_modifier);
    check->registerClickHandler(checkClick, this);

    // Velocity Weight Modifier:
    {
      GUI::Label *lbl_weight = new GUI::Label(window);
      lbl_weight->setText("Attack");
      lbl_weight->move(107, y);
      lbl_weight->resize(100, 20);
      
      knob = new GUI::Knob(window);
      knob->move(109, y + OFFSET4 - 4);
      knob->resize(57, 57);
      knob->setValue(Conf::velocity_modifier_weight);
      knob->registerClickHandler(knobChange, this);
    }
    
    // Velocity Falloff Modifier:
    {
      GUI::Label *lbl_falloff = new GUI::Label(window);
      lbl_falloff->setText("Release");
      lbl_falloff->move(202 - 17 - 7, y);
      lbl_falloff->resize(100, 20);
      
      knob2 = new GUI::Knob(window);
      knob2->move(202 - 13 - 5,  y + OFFSET4 - 4);
      knob2->resize(57, 57);
      knob2->setValue(Conf::velocity_modifier_falloff);
      knob2->registerClickHandler(knobChange2, this);
    }
  }

  GUI::VerticalLine *l2 = new GUI::VerticalLine(window);
  l2->move(20, 310 - 15 - 9);
  l2->resize(window->width() - 40, 2);

  GUI::Label *lbl_version = new GUI::Label(window);
  lbl_version->setText(".::. v"VERSION"  .::.  http://www.drumgizmo.org  .::.  GPLv3 .::.");
  lbl_version->move(16, 300);
  lbl_version->resize(window->width(), 20);

  {
    GUI::ComboBox *cmb = new GUI::ComboBox(window);
    cmb->addItem("Foo", "Bar");
    cmb->addItem("Hello", "World");
    cmb->move(10,10);
    cmb->resize(70, 30);
  }

  // Create filebrowser
  filebrowser = new GUI::FileBrowser(window);
  filebrowser->move(0, 0);
  filebrowser->resize(window->width() - 1, window->height() - 1);
  filebrowser->hide();
  fb = filebrowser;

  window->show();

  sem.post();
}

static bool shown = false;
void PluginGUI::show()
{
  if(!gctx) init();

  //  printf("PluginGUI::show()\n");
  if(!shown && window) {
    shown = true;
    //window->show();
  }
}

void PluginGUI::hide()
{
  //  printf("PluginGUI::hide()\n");
  if(window) window->hide();
}

void PluginGUI::processEvents()
{
#ifdef USE_THREAD
#else
  printf("PluginGUI::processEvents()\n");
  eventhandler->processEvents(window);
#endif/*USE_THREAD*/
}

void PluginGUI::setWindowClosedCallback(void (*handler)(void *), void *ptr)
{
  windowClosedHandler = handler;
  windowClosedPtr = ptr;
  eventhandler->registerCloseHandler(handler, ptr);
}


void PluginGUI::setChangeMidimapCallback(void (*handler)(void *, const char *),
                                         void *ptr)
{
  changeMidimapHandler = handler;
  changeMidimapPtr = ptr;
}

#ifdef STANDALONE

void stop(void *ptr)
{
  DEBUG(stop, "Stopping...\n");
  bool *running = (bool*)ptr;
  *running = false;
}

int main()
{
/*
  hug_status_t status = hug_init(HUG_FLAG_OUTPUT_TO_SYSLOG,
                                 HUG_OPTION_SYSLOG_HOST, "192.168.0.10",
                                 HUG_OPTION_SYSLOG_PORT, 514,
                                 HUG_OPTION_END);

  if(status != HUG_STATUS_OK) {
    printf("Error: %d\n", status);
    return 1;
  }
*/
  INFO(example, "We are up and running");

  bool running = true;

  PluginGUI gui(NULL);
  gui.setWindowClosedCallback(stop, &running);

  // gui.show();

  while(running) {
    //    gui.processEvents();
#ifdef WIN32
    SleepEx(1000, FALSE);
#else
    //    usleep(10000);
    sleep(1);
#endif
  }

  return 0;
}

#endif/*STANDALONE*/

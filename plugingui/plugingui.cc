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

#ifndef STANDALONE
#include <drumgizmo.h>
#include "../src/configuration.h"
#else
class DrumGizmo {
public:
  void loadkit(std::string) {}
  void init(bool) {}
  std::string drumkitfile() { return ""; }
  std::string midimapfile;
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
}

static void knobChange2(void *ptr)
{
  PluginGUI *gui = (PluginGUI*)ptr;
  Conf::velocity_modifier_falloff = gui->knob2->value();
}

GUI::FileBrowser *fb;
static void selectKitFile(void *ptr, std::string filename)
{
  PluginGUI *gui = (PluginGUI*)ptr;

  gui->lineedit->setText(filename);
  fb->hide();

  std::string drumkit = gui->lineedit->text();
  if(!gui->drumgizmo) return;
  gui->drumgizmo->loadkit(drumkit);
  gui->drumgizmo->init(true);
  gui->led->setState(GUI::LED::green);
}

static void kitBrowseClick(void *ptr)
{
  PluginGUI *gui = (PluginGUI*)ptr;

  fb->registerFileSelectHandler(selectKitFile, gui);
  fb->show();
}

static void selectMapFile(void *ptr, std::string filename)
{
  PluginGUI *gui = (PluginGUI*)ptr;

  gui->lineedit2->setText(filename);
  fb->hide();

  std::string midimap = gui->lineedit2->text();
  if(gui->changeMidimapHandler)
    gui->changeMidimapHandler(gui->changeMidimapPtr, midimap.c_str());
  gui->led2->setState(GUI::LED::green);
}

static void midimapBrowseClick(void *ptr)
{
  PluginGUI *gui = (PluginGUI*)ptr;

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
}

PluginGUI::~PluginGUI()
{
  printf("~PluginGUI()\n");

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
    //    printf("."); fflush(stdout);
  }
}

void PluginGUI::init()
{
  DEBUG(gui, "init");
  gctx = new GUI::GlobalContext();
  eventhandler = new GUI::EventHandler(gctx);
  //  printf("%p\n", eventhandler);
  window = new GUI::Window(gctx);
  window->resize(640, 200);

  // Enable Velocity
  check = new GUI::CheckBox(window);
  check->setText("Enable Velocity Modifier");
  check->move(210,82);
  check->resize(200,16);
  check->setChecked(Conf::enable_velocity_modifier);
  check->registerClickHandler(checkClick, this);

  // Velocity Weight Modifier:
  {
    int xpos = 180;
    GUI::Label *lbl_weight = new GUI::Label(window);
    lbl_weight->setText("Weight Modifier");
    lbl_weight->move(xpos, 138 + 15);
    lbl_weight->resize(100, 20);

    knob = new GUI::Knob(window);
    knob->move(xpos + 30, 100 + 15);
    knob->resize(41, 41);
    knob->setValue(Conf::velocity_modifier_weight);
    knob->registerClickHandler(knobChange, this);
  }

  // Velocity Falloff Modifier:
  {
    int xpos = 300;
    GUI::Label *lbl_falloff = new GUI::Label(window);
    lbl_falloff->setText("Falloff Modifier");
    lbl_falloff->move(xpos, 138 + 15);
    lbl_falloff->resize(100, 20);
    
    knob2 = new GUI::Knob(window);
    knob2->move(xpos + 30, 100 + 15);
    knob2->resize(41, 41);
    knob2->setValue(Conf::velocity_modifier_falloff);
    knob2->registerClickHandler(knobChange2, this);
  }

  // Drumkit file
  {
    GUI::Label *lbl = new GUI::Label(window);
    lbl->setText("Drumkit:");
    lbl->move(10, 10);
    lbl->resize(70, 20);

    led = new GUI::LED(window);
    led->move(600,12);
    led->resize(16, 16);
    //  led->setState(false);

    lineedit = new FileLineEdit(window, led);
    if(drumgizmo) lineedit->setText(drumgizmo->drumkitfile());
    else lineedit->setText("Missing DrumGizmo*");
    lineedit->move(70, 10);
    lineedit->resize(408, 20);
    
    GUI::Button *btn_brw = new GUI::Button(window);
    btn_brw->setText("Load Kit...");
    btn_brw->move(490, 10);
    btn_brw->resize(100, 20);
    btn_brw->registerClickHandler(kitBrowseClick, this);
  }

  // Midimap file
  {
    lbl2 = new GUI::Label(window);
    lbl2->setText("Midimap:");
    lbl2->move(10, 45);
    lbl2->resize(70, 20);
    
    led2 = new GUI::LED(window);
    led2->move(600,47);
    led2->resize(16, 16);
    //  led2->setState(false);
    
    lineedit2 = new FileLineEdit(window, led2);
    if(drumgizmo) lineedit2->setText(drumgizmo->midimapfile);
    lineedit2->move(70, 45);
    lineedit2->resize(408, 20);
    
    GUI::Button *btn_brw = new GUI::Button(window);
    btn_brw->setText("Load Map...");
    btn_brw->move(490, 45);
    btn_brw->resize(100, 20);
    btn_brw->registerClickHandler(midimapBrowseClick, this);
  }

  /*
  btn_cancel = new GUI::Button(window);
  btn_cancel->setText("Close");
  btn_cancel->move(520, 160);
  btn_cancel->resize(100, 20);
  btn_cancel->registerClickHandler(closeClick, this);
  */

  GUI::Label *lbl3 = new GUI::Label(window);
  lbl3->setText("v"VERSION);
  lbl3->move(120, 180);
  lbl3->resize(70, 20);

  // Create filebrowser
  filebrowser = new GUI::FileBrowser(window);
  filebrowser->move(0, 0);
  filebrowser->resize(window->width() - 1, window->height() - 1);
  filebrowser->hide();
  fb = filebrowser;

  window->show();
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
  //gui.setWindowClosedCallback(stop, &running);

  // gui.show();

  while(running) {
    //    gui.processEvents();
#ifdef WIN32
    SleepEx(1000, FALSE);
    printf("loop\n");
#else
    //    usleep(10000);
    sleep(1);
#endif
  }

  return 0;
}

#endif/*STANDALONE*/

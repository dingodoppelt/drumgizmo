/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            mainwindow.h
 *
 *  Tue Nov 10 10:21:03 CET 2009
 *  Copyright 2009 Bent Bisballe Nyeng
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
#ifndef __DRUMGIZMO_MAINWINDOW_H__
#define __DRUMGIZMO_MAINWINDOW_H__

#include <QMainWindow>
#include <QScrollBar>
#include <QComboBox>
#include <QSlider>
#include <QLineEdit>
#include <QCloseEvent>
#include <QTabWidget>

#include "canvas.h"
#include "audioextractor.h"
#include "samplesorter.h"
#include "filelist.h"
#include "canvastoolselections.h"
#include "canvastoolthreshold.h"
#include "canvastoollisten.h"
#include "selection.h"
#include "player.h"

class Preset {
public:  
  QString prefix;
  int attacklength;
  int falloff;
  int fadelength;
};
Q_DECLARE_METATYPE(Preset)

class MainWindow : public QMainWindow {
Q_OBJECT
public:
  MainWindow();
  ~MainWindow();

public slots:
  void setXScale(int);
  void setYScale(int);
  void setXOffset(int);
  void setYOffset(int);
  void doExport();
  void loadFile(QString filename);
  void playSamples();
  void setPreset(int);
  void browse();
  void tabChanged(int tabid);

protected:
  void closeEvent(QCloseEvent*);

private:
  void loadSettings();
  void saveSettings();

  QWidget *createFilesTab();
  QWidget *createEditTab();
  QWidget *createGenerateTab();
  QWidget *createExportTab();

  int generateTabId;

  SampleSorter *sorter;
  Canvas *canvas;
  CanvasToolSelections *tool_selections;
  CanvasToolThreshold *threshold;
  CanvasToolListen *listen;
  AudioExtractor *extractor;
  FileList *filelist;
  QScrollBar *yoffset;
  QScrollBar *yscale;
  QScrollBar *xscale;
  QScrollBar *xoffset;
  QScrollBar *sb_playsamples;
  QComboBox *presets;
  QSlider *slider_attacklength;
  QSlider *slider_hold;
  QSlider *slider_falloff;
  QSlider *slider_fadelength;
  QLineEdit *prefix;
  QLineEdit *lineed_exportp;

  QTabWidget *tabs;

  // Session state information:
  Selections selections;
  Selections selections_preview;
  Player player;
};

#endif/*__DRUMGIZMO_MAINWINDOW_H__*/

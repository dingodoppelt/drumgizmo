/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            mainwindow.cc
 *
 *  Tue Nov 10 10:21:04 CET 2009
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
#include "mainwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QPushButton>
#include <QLabel>
#include <QStatusBar>
#include <QApplication>
#include <QDockWidget>
#include <QSettings>
#include <QToolBar>
#include <QAction>
#include <QMenuBar>

#include "canvastool.h"
#include "canvastoolthreshold.h"
#include "canvastoollisten.h"

#define MAXVAL 10000000L
#define SINGLESTEP MAXVAL/100000
#define PAGESTEP MAXVAL/10000

static void addTool(QToolBar *toolbar, Canvas *canvas, CanvasTool *tool)
{
  QAction *action = new QAction(tool->name(), toolbar);
  action->setCheckable(true);
  toolbar->addAction(action);
  tool->connect(action, SIGNAL(toggled(bool)), tool, SLOT(setActive(bool)));
  tool->setActive(false);
  canvas->tools.push_back(tool);
}

MainWindow::MainWindow()
{
  QWidget *central = new QWidget();
  QHBoxLayout *lh = new QHBoxLayout();
  QVBoxLayout *lv = new QVBoxLayout();
  central->setLayout(lv);
  setCentralWidget(central);

  extractor = new AudioExtractor(this);
  canvas = new Canvas(this);

  QToolBar *toolbar = addToolBar("Tools");
  CanvasTool *listen = new CanvasToolListen(canvas);
  addTool(toolbar, canvas, listen);
  CanvasTool *threshold = new CanvasToolThreshold(canvas);
  addTool(toolbar, canvas, threshold);
  selections = new CanvasToolSelections(canvas);
  connect(threshold, SIGNAL(thresholdChanged(double)),
          selections, SLOT(thresholdChanged(double)));
  addTool(toolbar, canvas, selections);

  QMenu *fileMenu = menuBar()->addMenu("&File");
  QAction *act_quit = new QAction("&Quit", this);
  fileMenu->addAction(act_quit);
  connect(act_quit, SIGNAL(triggered()), this, SLOT(close()));

  QWidget *dock = new QWidget();
  yoffset = new QScrollBar(Qt::Vertical);
  yoffset->setRange(0, MAXVAL);
  yoffset->setPageStep(PAGESTEP);
  yoffset->setSingleStep(SINGLESTEP);
  connect(yoffset, SIGNAL(valueChanged(int)), this, SLOT(setYOffset(int)));

  yscale = new QScrollBar(Qt::Vertical);
  yscale->setRange(0, MAXVAL);
  yscale->setPageStep(PAGESTEP);
  yscale->setSingleStep(SINGLESTEP);
  connect(yscale, SIGNAL(valueChanged(int)), this, SLOT(setYScale(int)));

  xscale = new QScrollBar(Qt::Horizontal);
  xscale->setRange(0, MAXVAL);
  xscale->setPageStep(PAGESTEP);
  xscale->setSingleStep(SINGLESTEP);
  connect(xscale, SIGNAL(valueChanged(int)), this, SLOT(setXScale(int)));

  xoffset = new QScrollBar(Qt::Horizontal);
  xoffset->setRange(0, MAXVAL);
  xoffset->setPageStep(PAGESTEP);
  xoffset->setSingleStep(SINGLESTEP);
  connect(xoffset, SIGNAL(valueChanged(int)), this, SLOT(setXOffset(int)));

  sorter = new SampleSorter();
  connect(selections, SIGNAL(selectionsChanged(Selections)),
          sorter, SLOT(setSelections(Selections)));
  connect(selections, SIGNAL(activeSelectionChanged(Selection)),
          sorter, SLOT(setActiveSelection(Selection)));

  connect(sorter, SIGNAL(activeSelectionChanged(Selection)),
          selections, SLOT(setActiveSelection(Selection)));

  lh->addWidget(canvas);
  lh->addWidget(yscale);
  lh->addWidget(yoffset);
  lv->addLayout(lh, 100);
  lv->addWidget(xscale, 100);
  lv->addWidget(xoffset, 100);
  lv->addWidget(sorter, 15);


  QHBoxLayout *btns = new QHBoxLayout();

  QPushButton *autosel = new QPushButton();
  autosel->setText("Auto");
  connect(autosel, SIGNAL(clicked()), selections, SLOT(clearSelections()));
  connect(autosel, SIGNAL(clicked()), selections, SLOT(autoCreateSelections()));

  QPushButton *clearsel = new QPushButton();
  clearsel->setText("Clear");
  connect(clearsel, SIGNAL(clicked()), selections, SLOT(clearSelections()));

  QPushButton *exportsel = new QPushButton();
  exportsel->setText("Export");
  connect(exportsel, SIGNAL(clicked()), this, SLOT(doExport()));

  btns->addWidget(autosel);
  btns->addWidget(clearsel);
  btns->addWidget(exportsel);

  QVBoxLayout *configs = new QVBoxLayout();
  
  configs->addLayout(btns);

  configs->addWidget(new QLabel("Presets:"));
  presets = new QComboBox();
  connect(presets, SIGNAL(currentIndexChanged(int)), this, SLOT(setPreset(int)));
  configs->addWidget(presets);

  configs->addWidget(new QLabel("Attack length:"));
  slider_attacklength = new QSlider(Qt::Horizontal);
  slider_attacklength->setRange(10, 1000);
  connect(slider_attacklength, SIGNAL(sliderMoved(int)), sorter, SLOT(setAttackLength(int)));
  slider_attacklength->setValue(666);
  configs->addWidget(slider_attacklength);

  configs->addWidget(new QLabel("Falloff:"));
  slider_falloff = new QSlider(Qt::Horizontal);
  slider_falloff->setRange(1, 10000);
  connect(slider_falloff, SIGNAL(sliderMoved(int)),
          selections, SLOT(noiseFloorChanged(int)));
  slider_falloff->setValue(666);
  configs->addWidget(slider_falloff); 

  configs->addWidget(new QLabel("Fadelength:"));
  slider_fadelength = new QSlider(Qt::Horizontal);
  slider_fadelength->setRange(1, 2000);
  connect(slider_fadelength, SIGNAL(sliderMoved(int)),
          selections, SLOT(fadeoutChanged(int)));
  slider_fadelength->setValue(666);
  configs->addWidget(slider_fadelength); 

  configs->addWidget(new QLabel("Player volume:"));
  QSlider *slider4 = new QSlider(Qt::Horizontal);
  slider4->setRange(0, 1000000);
  connect(slider4, SIGNAL(sliderMoved(int)),
          listen, SLOT(setVolume(int)));
  slider4->setValue(100000);
  configs->addWidget(slider4); 

  configs->addWidget(new QLabel("Prefix:"));
  prefix = new QLineEdit();
  connect(prefix, SIGNAL(textChanged(const QString &)),
          extractor, SLOT(setOutputPrefix(const QString &)));
  prefix->setText("kick-r");
  configs->addWidget(prefix);

  configs->addWidget(new QLabel("Export path:"));
  QLineEdit *exportp = new QLineEdit();
  connect(exportp, SIGNAL(textChanged(const QString &)),
          extractor, SLOT(setExportPath(const QString &)));
  exportp->setText("/home/deva/tmp/drumgizmoexport");
  configs->addWidget(exportp);

  QPushButton *loadbtn = new QPushButton();
  loadbtn->setText("Add files...");
  configs->addWidget(loadbtn);

  configs->addWidget(new QLabel("Files: (double-click to set as master)"));
  filelist = new FileList();
  connect(filelist, SIGNAL(masterFileChanged(QString)),
          this, SLOT(loadFile(QString)));
  connect(loadbtn, SIGNAL(clicked()), filelist, SLOT(addFiles()));
  connect(filelist, SIGNAL(fileAdded(QString, QString)),
          extractor, SLOT(addFile(QString, QString)));
  connect(filelist, SIGNAL(fileRemoved(QString, QString)),
          extractor, SLOT(removeFile(QString, QString)));
  connect(filelist, SIGNAL(nameChanged(QString, QString)),
          extractor, SLOT(changeName(QString, QString)));
  configs->addWidget(filelist);

  QDockWidget *dockWidget = new QDockWidget(tr("Dock Widget"), this);
  dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  dockWidget->setWidget(dock);
  addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
  dock->setLayout(configs);

  yscale->setValue(MAXVAL);
  yoffset->setValue(MAXVAL/2);
  xscale->setValue(0);
  xoffset->setValue(0);
  
  loadSettings();

  QSettings settings("presets.ini", QSettings::IniFormat);
  QStringList list = settings.childGroups();
  for(int i = 0; i != list.size(); i++) {
    QString presetname = list.at(i);
    Preset p;
    settings.beginGroup(presetname);
    p.name = presetname;
    p.attacklength = settings.value("attacklength", 0).toInt();
    p.falloff = settings.value("falloff", 0).toInt();
    p.fadelength = settings.value("fadelength", 0).toInt();
    settings.endGroup();
    QVariant v;
    v.setValue(p);
    presets->addItem(presetname, v); 
  }

  statusBar()->showMessage("Ready");
}

void MainWindow::closeEvent(QCloseEvent *)
{
  saveSettings();
  QApplication::quit();
}

void MainWindow::loadSettings()
{
  QSettings settings("Aasimon.org", "DGEdit");

  settings.beginGroup("MainWindow");
  resize(settings.value("size", QSize(700, 800)).toSize());
  move(settings.value("pos", QPoint(0, 0)).toPoint());
  settings.endGroup();
}

void MainWindow::saveSettings()
{
  QSettings settings("Aasimon.org", "DGEdit");

  settings.beginGroup("MainWindow");
  settings.setValue("size", size());
  settings.setValue("pos", pos());
  settings.endGroup();
}

void MainWindow::setXScale(int sz)
{
  // range 0.0 - 1.0
  float val = (float)(sz * -1 + MAXVAL)/(float)MAXVAL;
  canvas->setXScale(val);
  if(val < 0.001) val = 0.001;
  xoffset->setPageStep(PAGESTEP * 10 * val);
  xoffset->setSingleStep(SINGLESTEP * 10 * val);
}

void MainWindow::setYScale(int sz)
{
  // range 0.0 - 1.0
  float val = (float)(sz * -1 + MAXVAL)/(float)MAXVAL;
  canvas->setYScale(val);
}

void MainWindow::setXOffset(int of)
{
  // range 0.0 - 1.0
  float val = (float)of/(float)MAXVAL;
  canvas->setXOffset(val);
}

void MainWindow::setYOffset(int of)
{
  // range 0.0 - 1.0
  float val = (float)(of * -1 + MAXVAL)/(float)MAXVAL;
  canvas->setYOffset(val);
}

void MainWindow::doExport()
{
  extractor->exportSelections(sorter->selections(), sorter->levels());
}

void MainWindow::loadFile(QString filename)
{
  setCursor(Qt::WaitCursor);
  statusBar()->showMessage("Loading...");
  qApp->processEvents();
  sorter->setWavData(NULL, 0);
  canvas->load(filename);
  sorter->setWavData(canvas->data, canvas->size);
  statusBar()->showMessage("Ready");
  setCursor(Qt::ArrowCursor);
}

void MainWindow::setPreset(int index) 
{
  QVariant v = presets->itemData(index);
  Preset p = v.value<Preset>();
  slider_attacklength->setValue(p.attacklength);
  slider_falloff->setValue(p.falloff);
  slider_fadelength->setValue(p.fadelength);
  prefix->setText(p.name);
//  sorter->setAttackLength(p.attacklength);  
//  selections->noiseFloorChanged(p.falloff);
//  selections->fadeoutChanged(p.fadeout);
}

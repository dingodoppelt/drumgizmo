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
#include <QFileDialog>
#include <QIntValidator>

#include <unistd.h>

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

static CanvasToolListen *g_listen;
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
  g_listen = new CanvasToolListen(canvas);
  CanvasTool *listen = g_listen;
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

  QPushButton *btn_playsamples = new QPushButton("Play samples");
  connect(btn_playsamples, SIGNAL(clicked()), this, SLOT(playSamples()));
  
  sb_playsamples = new QScrollBar(Qt::Horizontal);
  sb_playsamples->setRange(100, 4000); // ms
  

  lh->addWidget(canvas);
  lh->addWidget(yscale);
  lh->addWidget(yoffset);
  lv->addLayout(lh, 100);
  lv->addWidget(xscale, 100);
  lv->addWidget(xoffset, 100);
  lv->addWidget(sorter, 15);
  lv->addWidget(btn_playsamples);
  lv->addWidget(sb_playsamples);

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

  QGridLayout *attribs_layout = new QGridLayout();

  attribs_layout->addWidget(new QLabel("Attack length:"), 1, 1, 1, 2);
  lineed_attacklength = new QLineEdit();
  lineed_attacklength->setReadOnly(true);
  lineed_attacklength->setValidator(new QIntValidator(0, 1000));
  attribs_layout->addWidget(lineed_attacklength, 2, 1);
  slider_attacklength = new QSlider(Qt::Horizontal);
  slider_attacklength->setRange(10, 1000);
  connect(slider_attacklength, SIGNAL(sliderMoved(int)),
          this, SLOT(setAttackLengthLineEd(int)));
  connect(slider_attacklength, SIGNAL(sliderMoved(int)),
          sorter, SLOT(setAttackLength(int)));
  slider_attacklength->setValue(666);
  attribs_layout->addWidget(slider_attacklength, 2, 2);

  attribs_layout->addWidget(new QLabel("Falloff:"), 3, 1, 1, 2);
  lineed_falloff = new QLineEdit();
  lineed_falloff->setReadOnly(true);
  lineed_falloff->setValidator(new QIntValidator(0, 10000));
  attribs_layout->addWidget(lineed_falloff, 4, 1);
  slider_falloff = new QSlider(Qt::Horizontal);
  slider_falloff->setRange(1, 10000);
  connect(slider_falloff, SIGNAL(sliderMoved(int)),
          this, SLOT(setFalloffLineEd(int)));
  connect(slider_falloff, SIGNAL(sliderMoved(int)),
          selections, SLOT(noiseFloorChanged(int)));
  slider_falloff->setValue(666);
  attribs_layout->addWidget(slider_falloff, 4, 2);

  attribs_layout->addWidget(new QLabel("Fadelength:"), 5, 1, 1, 2);
  lineed_fadelength = new QLineEdit();
  lineed_fadelength->setReadOnly(true);
  lineed_fadelength->setValidator(new QIntValidator(0, 2000));
  attribs_layout->addWidget(lineed_fadelength, 6, 1);
  slider_fadelength = new QSlider(Qt::Horizontal);
  slider_fadelength->setRange(1, 2000);
  connect(slider_fadelength, SIGNAL(sliderMoved(int)),
          this, SLOT(setFadeLengthLineEd(int)));
  connect(slider_fadelength, SIGNAL(sliderMoved(int)),
          selections, SLOT(fadeoutChanged(int)));
  slider_fadelength->setValue(666);
  attribs_layout->addWidget(slider_fadelength, 6, 2);

  attribs_layout->addWidget(new QLabel("Player volume:"), 7, 1, 1, 2);
  lineed_slider4 = new QLineEdit();
  lineed_slider4->setReadOnly(true);
  lineed_slider4->setValidator(new QIntValidator(0, 1000000));
  attribs_layout->addWidget(lineed_slider4, 8, 1);
  QSlider *slider4 = new QSlider(Qt::Horizontal);
  slider4->setRange(0, 1000000);
  connect(slider4, SIGNAL(sliderMoved(int)),
          this, SLOT(setVolumeLineEd(int)));
  connect(slider4, SIGNAL(sliderMoved(int)),
          listen, SLOT(setVolume(int)));
  slider4->setValue(100000);
  attribs_layout->addWidget(slider4, 8, 2); 

  configs->addLayout(attribs_layout);

  configs->addWidget(new QLabel("Prefix:"));
  prefix = new QLineEdit();
  connect(prefix, SIGNAL(textChanged(const QString &)),
          extractor, SLOT(setOutputPrefix(const QString &)));
  configs->addWidget(prefix);

  configs->addWidget(new QLabel("Export path:"));
  QHBoxLayout *lo_exportp = new QHBoxLayout();
  lineed_exportp = new QLineEdit();
  connect(lineed_exportp, SIGNAL(textChanged(const QString &)),
          extractor, SLOT(setExportPath(const QString &)));
  lo_exportp->addWidget(lineed_exportp);
  QPushButton *btn_browse = new QPushButton("...");
  connect(btn_browse, SIGNAL(clicked()), this, SLOT(browse()));
  lo_exportp->addWidget(btn_browse);

  configs->addLayout(lo_exportp);

  configs->addWidget(new QLabel("Files: (double-click to set as master)"));
  QPushButton *loadbtn = new QPushButton();
  loadbtn->setText("Add files...");
  configs->addWidget(loadbtn);

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
    p.prefix = settings.value("prefix", "unknown").toString();
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

void MainWindow::setAttackLengthLineEd(int value)
{
  lineed_attacklength->setText(QString::number(value));
}

void MainWindow::setFalloffLineEd(int value)
{
  lineed_falloff->setText(QString::number(value));
}

void MainWindow::setFadeLengthLineEd(int value)
{
  lineed_fadelength->setText(QString::number(value));
}

void MainWindow::setVolumeLineEd(int value)
{
  lineed_slider4->setText(QString::number(value));
}


void MainWindow::playSamples()
{
  //  unsigned int length = 44100 / 4; // 0.25 seconds in 44k1Hz

  Selections sels = sorter->selections();
  Selections::iterator i = sels.begin();
  while(i != sels.end()) {
    unsigned int length = sb_playsamples->value() * 44100 / 1000;

    unsigned int sample_length = i->to - i->from;

    unsigned int to = i->to;
    unsigned int sleep = 0;

    if(sample_length > length) to = i->from + length;
    else sleep = length - sample_length;
    g_listen->playRange(i->from, to);
    usleep(1000000 * sleep / 44100);
    i++;
  }
}

void MainWindow::closeEvent(QCloseEvent *)
{
  saveSettings();
  QApplication::quit();
}

void MainWindow::loadSettings()
{
  QSettings settings("config.ini", QSettings::IniFormat);

  settings.beginGroup("MainWindow");
  lineed_exportp->setText(settings.value("exportpath", "").toString());
  resize(settings.value("size", QSize(700, 800)).toSize());
  move(settings.value("pos", QPoint(0, 0)).toPoint());
  settings.endGroup();
}

void MainWindow::saveSettings()
{
  QSettings settings("config.ini", QSettings::IniFormat);

  settings.beginGroup("MainWindow");
  settings.setValue("exportpath", lineed_exportp->text());
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
  prefix->setText(p.prefix);
}

void MainWindow::browse() {
  QString path = QFileDialog::getExistingDirectory(this, "Select export path", lineed_exportp->text());
  lineed_exportp->setText(path);
}

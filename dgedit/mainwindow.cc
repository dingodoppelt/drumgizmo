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
#include <QLineEdit>
#include <QLabel>
#include <QStatusBar>
#include <QApplication>
#include <QDockWidget>
#include <QSettings>

#define MAXVAL 10000000L
#define SINGLESTEP MAXVAL/100000
#define PAGESTEP MAXVAL/10000

MainWindow::MainWindow()
{
  QWidget *central = new QWidget();
  QHBoxLayout *lh = new QHBoxLayout();
  QVBoxLayout *lv = new QVBoxLayout();
  central->setLayout(lv);
  setCentralWidget(central);

  extractor = new AudioExtractor(this);
  canvas = new Canvas(this);

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
  connect(canvas, SIGNAL(selectionsChanged(Selections)),
          sorter, SLOT(setSelections(Selections)));
  connect(canvas, SIGNAL(activeSelectionChanged(Selection)),
          sorter, SLOT(setActiveSelection(Selection)));

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
  connect(autosel, SIGNAL(clicked()), canvas, SLOT(clearSelections()));
  connect(autosel, SIGNAL(clicked()), canvas, SLOT(autoCreateSelections()));

  QPushButton *clearsel = new QPushButton();
  clearsel->setText("Clear");
  connect(clearsel, SIGNAL(clicked()), canvas, SLOT(clearSelections()));

  QPushButton *exportsel = new QPushButton();
  exportsel->setText("Export");
  connect(exportsel, SIGNAL(clicked()), this, SLOT(doExport()));

  btns->addWidget(autosel);
  btns->addWidget(clearsel);
  btns->addWidget(exportsel);

  QVBoxLayout *configs = new QVBoxLayout();
  
  configs->addLayout(btns);

  configs->addWidget(new QLabel("Prefix:"));
  QLineEdit *prefix = new QLineEdit();
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

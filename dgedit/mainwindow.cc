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
#include <QTabWidget>

#include <unistd.h>

#include "canvastool.h"
#include "canvastoolthreshold.h"
#include "canvastoollisten.h"
#include "volumefader.h"

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
  {
    int start = 44100 * 60;
    Selection p(start, start + 44100 * 60, 0, 0); // one minute selection
    selections_preview.add(p);
  }

  QWidget *central = new QWidget();
  QHBoxLayout *lh = new QHBoxLayout();
  QVBoxLayout *lv = new QVBoxLayout();
  central->setLayout(lv);
  setCentralWidget(central);

  extractor = new AudioExtractor(selections, this);
  canvas = new Canvas(this);

  QToolBar *toolbar = addToolBar("Tools");
  g_listen = new CanvasToolListen(canvas, player);
  CanvasTool *listen = g_listen;
  addTool(toolbar, canvas, listen);
  threshold = new CanvasToolThreshold(canvas);
  addTool(toolbar, canvas, threshold);
  tool_selections = new CanvasToolSelections(canvas, selections,
                                             selections_preview);
  connect(threshold, SIGNAL(thresholdChanged(double)),
          tool_selections, SLOT(thresholdChanged(double)));
  connect(&selections, SIGNAL(activeChanged(sel_id_t)),
          canvas, SLOT(update()));
  addTool(toolbar, canvas, tool_selections);

  QMenu *fileMenu = menuBar()->addMenu("&File");
  QAction *act_quit = new QAction("&Quit", this);
  fileMenu->addAction(act_quit);
  connect(act_quit, SIGNAL(triggered()), this, SLOT(close()));

  //  QWidget *dock = new QWidget();
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

  sorter = new SampleSorter(selections, selections_preview);
  connect(&selections, SIGNAL(added(sel_id_t)),
          sorter, SLOT(addSelection(sel_id_t)));
  connect(&selections_preview, SIGNAL(added(sel_id_t)),
          sorter, SLOT(addSelectionPreview(sel_id_t)));
  connect(&selections, SIGNAL(updated(sel_id_t)), sorter, SLOT(relayout()));
  connect(&selections_preview, SIGNAL(updated(sel_id_t)),
          sorter, SLOT(relayout()));
  connect(&selections, SIGNAL(removed(sel_id_t)), sorter, SLOT(relayout()));
  connect(&selections_preview, SIGNAL(removed(sel_id_t)),
          sorter, SLOT(relayout()));
  connect(&selections, SIGNAL(activeChanged(sel_id_t)),
          sorter, SLOT(relayout()));

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


  QDockWidget *dockWidget = new QDockWidget(tr("Dock Widget"), this);
  dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  dockWidget->setWidget(new QWidget());
  dockWidget->widget()->setLayout(new QVBoxLayout());

  QTabWidget *tabs = new QTabWidget(this);
  tabs->addTab(createFilesTab(), "Files");
  generateTabId = tabs->addTab(createGenerateTab(), "Generate");
  tabs->addTab(createEditTab(), "Edit");
  tabs->addTab(createExportTab(), "Export");
  connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
  tabChanged(tabs->currentIndex());

  dockWidget->widget()->layout()->addWidget(tabs);

  VolumeFader *vol = new VolumeFader();
  connect(vol, SIGNAL(volumeChangedDb(double)),
          &player, SLOT(setGainDB(double)));
  connect(&player, SIGNAL(peakUpdate(double)),
          vol, SLOT(updatePeakPower(double)));
  dockWidget->widget()->layout()->addWidget(vol);

  addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

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

MainWindow::~MainWindow()
{
}

void MainWindow::tabChanged(int tabid)
{
  tool_selections->setShowPreview(tabid == generateTabId);
  sorter->setShowPreview(tabid == generateTabId);
  tool_selections->autoCreateSelectionsPreview();
}

QWidget *MainWindow::createFilesTab()
{
  QWidget *w = new QWidget();
  QVBoxLayout *l = new QVBoxLayout();
  w->setLayout(l);

  l->addWidget(new QLabel("Files: (double-click to set as master)"));
  QPushButton *loadbtn = new QPushButton();
  loadbtn->setText("Add files...");
  l->addWidget(loadbtn);

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
  l->addWidget(filelist);

  return w;
}

QWidget *MainWindow::createEditTab()
{
  return new QWidget();
}

QWidget *MainWindow::createGenerateTab()
{
  QWidget *w = new QWidget();
  QVBoxLayout *l = new QVBoxLayout();
  w->setLayout(l);

  QHBoxLayout *btns = new QHBoxLayout();

  QPushButton *autosel = new QPushButton();
  autosel->setText("Generate");
  connect(autosel, SIGNAL(clicked()),
          tool_selections, SLOT(clearSelections()));
  connect(autosel, SIGNAL(clicked()),
          tool_selections, SLOT(autoCreateSelections()));

  connect(threshold, SIGNAL(thresholdChanged(double)),
          tool_selections, SLOT(autoCreateSelectionsPreview()));

  QPushButton *clearsel = new QPushButton();
  clearsel->setText("Clear");
  connect(clearsel, SIGNAL(clicked()),
          tool_selections, SLOT(clearSelections()));

  btns->addWidget(autosel);
  btns->addWidget(clearsel);
  
  l->addLayout(btns);

  l->addWidget(new QLabel("Presets:"));
  presets = new QComboBox();
  connect(presets, SIGNAL(currentIndexChanged(int)),
          this, SLOT(setPreset(int)));
  l->addWidget(presets);

  QGridLayout *attribs_layout = new QGridLayout();

  attribs_layout->addWidget(new QLabel("Attack length:"), 1, 1, 1, 2);
  lineed_attacklength = new QLineEdit();
  lineed_attacklength->setReadOnly(true);
  lineed_attacklength->setValidator(new QIntValidator(0, 1000,
                                                      lineed_attacklength));
  attribs_layout->addWidget(lineed_attacklength, 2, 1);
  slider_attacklength = new QSlider(Qt::Horizontal);
  slider_attacklength->setRange(10, 1000);
  connect(slider_attacklength, SIGNAL(sliderMoved(int)),
          this, SLOT(setAttackLengthLineEd(int)));
  connect(slider_attacklength, SIGNAL(sliderMoved(int)),
          sorter, SLOT(setAttackLength(int)));
  connect(slider_attacklength, SIGNAL(sliderMoved(int)),
          tool_selections, SLOT(autoCreateSelectionsPreview()));

  slider_attacklength->setValue(666);
  attribs_layout->addWidget(slider_attacklength, 2, 2);

  attribs_layout->addWidget(new QLabel("Falloff:"), 3, 1, 1, 2);
  lineed_falloff = new QLineEdit();
  lineed_falloff->setReadOnly(true);
  lineed_falloff->setValidator(new QIntValidator(0, 1000, lineed_falloff));
  attribs_layout->addWidget(lineed_falloff, 4, 1);
  slider_falloff = new QSlider(Qt::Horizontal);
  slider_falloff->setRange(1, 1000);
  connect(slider_falloff, SIGNAL(sliderMoved(int)),
          this, SLOT(setFalloffLineEd(int)));
  connect(slider_falloff, SIGNAL(sliderMoved(int)),
          tool_selections, SLOT(noiseFloorChanged(int)));
  connect(slider_falloff, SIGNAL(sliderMoved(int)),
          tool_selections, SLOT(autoCreateSelectionsPreview()));

  slider_falloff->setValue(666);
  attribs_layout->addWidget(slider_falloff, 4, 2);

  attribs_layout->addWidget(new QLabel("Fadelength:"), 5, 1, 1, 2);
  lineed_fadelength = new QLineEdit();
  lineed_fadelength->setReadOnly(true);
  lineed_fadelength->setValidator(new QIntValidator(0, 2000,
                                                    lineed_fadelength));
  attribs_layout->addWidget(lineed_fadelength, 6, 1);
  slider_fadelength = new QSlider(Qt::Horizontal);
  slider_fadelength->setRange(1, 2000);
  connect(slider_fadelength, SIGNAL(sliderMoved(int)),
          this, SLOT(setFadeLengthLineEd(int)));
  connect(slider_fadelength, SIGNAL(sliderMoved(int)),
          tool_selections, SLOT(fadeoutChanged(int)));
  connect(slider_fadelength, SIGNAL(sliderMoved(int)),
          tool_selections, SLOT(autoCreateSelectionsPreview()));

  slider_fadelength->setValue(666);
  attribs_layout->addWidget(slider_fadelength, 6, 2);

  l->addLayout(attribs_layout);

  l->addStretch();

  return w;
}

QWidget *MainWindow::createExportTab()
{
  QWidget *w = new QWidget();
  QVBoxLayout *l = new QVBoxLayout();
  w->setLayout(l);

  l->addWidget(new QLabel("Prefix:"));
  prefix = new QLineEdit();
  connect(prefix, SIGNAL(textChanged(const QString &)),
          extractor, SLOT(setOutputPrefix(const QString &)));
  l->addWidget(prefix);

  l->addWidget(new QLabel("Export path:"));
  QHBoxLayout *lo_exportp = new QHBoxLayout();
  lineed_exportp = new QLineEdit();
  connect(lineed_exportp, SIGNAL(textChanged(const QString &)),
          extractor, SLOT(setExportPath(const QString &)));
  lo_exportp->addWidget(lineed_exportp);
  QPushButton *btn_browse = new QPushButton("...");
  connect(btn_browse, SIGNAL(clicked()), this, SLOT(browse()));
  lo_exportp->addWidget(btn_browse);

  l->addLayout(lo_exportp);

  QPushButton *exportsel = new QPushButton();
  exportsel->setText("Export");
  connect(exportsel, SIGNAL(clicked()), this, SLOT(doExport()));
  l->addWidget(exportsel);

  l->addStretch();

  return w;
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
  //unsigned int length = 44100 / 4; // 0.25 seconds in 44k1Hz

  QVector<sel_id_t> ids = selections.ids();
  for(int v1 = 0; v1 < ids.size(); v1++) {
    for(int v2 = 0; v2 < ids.size(); v2++) {

      Selection sel1 = selections.get(ids[v1]);
      Selection sel2 = selections.get(ids[v2]);

      if(sel1.energy < sel2.energy) {
        sel_id_t vtmp = ids[v1];
        ids[v1] = ids[v2];
        ids[v2] = vtmp;
      }
    }
  }

  QVector<sel_id_t>::iterator i = ids.begin();
  while(i != ids.end()) {
    Selection sel = selections.get(*i);

    unsigned int length = sb_playsamples->value() * 44100 / 1000;

    unsigned int sample_length = sel.to - sel.from;

    unsigned int to = sel.to;
    unsigned int sleep = 0;

    if(sample_length > length) to = sel.from + length;
    else sleep = length - sample_length;

    selections.setActive(*i);
    
    g_listen->playRange(sel.from, to);
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
  extractor->exportSelections();
}

void MainWindow::loadFile(QString filename)
{
  setCursor(Qt::WaitCursor);
  statusBar()->showMessage("Loading...");
  qApp->processEvents();

  sorter->setWavData(NULL, 0);
  player.setPcmData(NULL, 0);

  canvas->load(filename);

  sorter->setWavData(canvas->data, canvas->size);
  player.setPcmData(canvas->data, canvas->size);

  statusBar()->showMessage("Ready");
  setCursor(Qt::ArrowCursor);
}

void MainWindow::setPreset(int index) 
{
  QVariant v = presets->itemData(index);
  Preset p = v.value<Preset>();
  slider_attacklength->setValue(p.attacklength);
  lineed_attacklength->setText(QString::number(p.attacklength));
  slider_falloff->setValue(p.falloff);
  lineed_falloff->setText(QString::number(p.falloff));
  slider_fadelength->setValue(p.fadelength);
  lineed_fadelength->setText(QString::number(p.fadelength));
  prefix->setText(p.prefix);
}

void MainWindow::browse() {
  QString path = QFileDialog::getExistingDirectory(this, "Select export path",
                                                   lineed_exportp->text());
  lineed_exportp->setText(path);
}

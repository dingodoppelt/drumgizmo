/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            filelist.cc
 *
 *  Mon Nov 30 15:35:52 CET 2009
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
#include "filelist.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>

FileList::FileList()
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(popupMenu(const QPoint &)));

  connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
          this, SLOT(selectionChanged(QListWidgetItem *)));

  createMenus();
}

void FileList::addFiles()
{
  QStringList files = QFileDialog::getOpenFileNames(this, tr("Open file"),
                                                    "", tr("Audio Files (*.wav)"));
  QStringList::Iterator i = files.begin();
  while(i != files.end()) {
    QString file = *i;
    QFileInfo fi(file);
    QString name = fi.baseName();

    QListWidgetItem *item = new QListWidgetItem();
    item->setText(file);
    item->setData(Qt::UserRole, name);
    item->setIcon(QPixmap("icons/file.png"));
    addItem(item);
    
    emit fileAdded(file, name);

    i++;
  }
}

void FileList::setMasterFile(QListWidgetItem *i)
{
  QString filename = i->text();

  for(int idx = 0; idx < count(); idx++) {
    item(idx)->setIcon(QPixmap("icons/file.png"));
  }

  i->setIcon(QPixmap("icons/master.png"));
  emit masterFileChanged(filename);
}

void FileList::selectionChanged(QListWidgetItem *i)
{
  setMasterFile(i);
}


void FileList::createMenus()
{
  menu = new QMenu();

  setMasterAction = new QAction("Set as Master (dbl-click)", this);
  connect(setMasterAction, SIGNAL(triggered()), this, SLOT(setMaster()));

  editAction = new QAction("Edit name", this);
  connect(editAction, SIGNAL(triggered()), this, SLOT(editName()));

  removeAction = new QAction("Remove", this);
  connect(removeAction, SIGNAL(triggered()), this, SLOT(removeFile()));

  menu->addAction(setMasterAction);
  menu->addAction(editAction);
  menu->addAction(removeAction);
}

void FileList::popupMenu(const QPoint & pos)
{
  activeItem = itemAt(pos);
  if(!activeItem) return;
  menu->popup(mapToGlobal(pos));
}

void FileList::setMaster()
{
  setMasterFile(activeItem);
}

void FileList::removeFile()
{
  QString file = activeItem->text();
  QString name = activeItem->data(Qt::UserRole).toString();

  printf("Removing: %s\n", file.toStdString().c_str());
  delete activeItem;//takeItem(row(activeItem));
  activeItem = NULL;
  setCurrentRow(-1);

  emit fileRemoved(file, name);
}

void FileList::editName()
{
}

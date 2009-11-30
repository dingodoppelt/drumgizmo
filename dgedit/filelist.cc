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

FileList::FileList()
{
  connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
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
    addItem(item);
    
    emit fileAdded(file, name);

    i++;
  }
}

void FileList::selectionChanged()
{
  QString filename = currentItem()->text();
  emit masterFileChanged(filename);
}

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            filelist.h
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
#ifndef __DRUMGIZMO_FILELIST_H__
#define __DRUMGIZMO_FILELIST_H__

#include <QListWidget>
#include <QListWidgetItem>
#include <QAction>
#include <QMenu>

class FileList : public QListWidget {
Q_OBJECT
public:
  FileList();

signals:
  void masterFileChanged(QString filename);
  void fileAdded(QString file, QString name);
  void fileRemoved(QString file, QString name);
  void nameChanged(QString file, QString name);

public slots:
  void addFiles();
  void popupMenu(const QPoint &pos);

private slots:
  void selectionChanged(QListWidgetItem *item);
  void setMaster();
  void removeFile();
  void editName();
  void setItemName(QListWidgetItem *i, QString name);

private:
  QString itemFile(QListWidgetItem *i);
  QString itemName(QListWidgetItem *i);
  void setItemFile(QListWidgetItem *i, QString file);
  void setItemMaster(QListWidgetItem *i, bool master);

  void setMasterFile(QListWidgetItem *i);
  void createMenus();

  QMenu *menu;
  QAction *setMasterAction;
  QAction *editAction;
  QAction *removeAction;

  QListWidgetItem *activeItem;
};

#endif/*__DRUMGIZMO_FILELIST_H__*/

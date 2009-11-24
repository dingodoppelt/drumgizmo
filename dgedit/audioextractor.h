/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audioextractor.h
 *
 *  Sat Nov 21 13:09:35 CET 2009
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
#ifndef __DRUMGIZMO_AUDIOEXTRACTOR_H__
#define __DRUMGIZMO_AUDIOEXTRACTOR_H__

#include <QObject>
#include <QVector>
#include <QPair>
#include <QString>

#include "selection.h"

class AudioExtractor : public QObject {
Q_OBJECT
public:
  AudioExtractor(QObject *parent);

  void addFile(QString file, QString name);
  void removeFile(QString file);

public slots:
  void exportSelections(Selections selections);
  void setExportPath(const QString &path);
  void setOutputPrefix(const QString &prefix);

private:
  float *load(QString file, size_t *size);
  void exportSelection(QString name, int index, float *data, size_t size, Selection sel);
  QVector< QPair<QString, QString> > audiofiles;
  QString exportpath;
  QString prefix;
};

#endif/*__DRUMGIZMO_AUDIOEXTRACTOR_H__*/

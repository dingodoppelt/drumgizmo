/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            selectioneditor.h
 *
 *  Thu Apr 17 17:25:17 CEST 2014
 *  Copyright 2014 Bent Bisballe Nyeng
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
#ifndef __DRUMGIZMO_SELECTIONEDITOR_H__
#define __DRUMGIZMO_SELECTIONEDITOR_H__

#include <QWidget>

#include <QLineEdit>
#include "selection.h"

class SelectionEditor : public QWidget {
Q_OBJECT
public:
  SelectionEditor(Selections &selections);

public slots:
  void added(sel_id_t id);
  void updated(sel_id_t id);
  void removed(sel_id_t id);
  void activeChanged(sel_id_t id);

private slots:
  void update();
  void updateSelection();

private:
  Selections &selections;

  sel_id_t cur;

  QLineEdit *from;
  QLineEdit *to;
  QLineEdit *fadein;
  QLineEdit *fadeout;
  QLineEdit *energy;
  QLineEdit *name;
};

#endif/*__DRUMGIZMO_SELECTIONEDITOR_H__*/

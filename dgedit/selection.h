/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            selection.h
 *
 *  Sat Nov 21 13:20:46 CET 2009
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
#ifndef __DRUMGIZMO_SELECTION_H__
#define __DRUMGIZMO_SELECTION_H__

#include <QObject>

#include <QMap>
#include <QVector>

class Selection {
public:
  Selection(int from = 0, int to = 0, int fadein = 0, int fadeout = 0) {
    this->from = from;
    this->to = to;
    this->fadein = fadein;
    this->fadeout = fadeout;
  }
  size_t from;
  size_t to;
  size_t fadein;
  size_t fadeout;

  double energy;

  QString name;
};

typedef int sel_id_t;
#define SEL_NONE -1

class Selections : public QObject {
Q_OBJECT
public:
  Selections();

  /**
   * Add a new selection object. The new id is returned.
   * Adding a new selections will emit an added signal with the new id.
   */
  sel_id_t add(Selection selection);

  /**
   * Get a stack copy of a specific selection object, by id.
   * NOTE: If id does not exist an empty selection (from = to = 0) is
   * returned.
   */
  Selection get(sel_id_t id);

  /**
   * Return vector (unsorted) of all ids in the object.
   */
  QVector<sel_id_t> ids();

  /**
   * Set active selection (to be rendered yellow)
   */
  void setActive(sel_id_t id);

  /**
   * Get active selection id.
   */
  sel_id_t active();

public slots:
  /**
   * Update a selection by id.
   * Updating a selection will emit a updated signal.
   */
  void update(sel_id_t id, Selection selection);

  /**
   * Delete a selection by id.
   * Deleting a selection will emit a deleted signal.
   */
  void remove(sel_id_t id);

  /**
   * Delete all selections
   */
  void clear();

signals:
  /**
   * This signal is emitted when a new selection has been added.
   */
  void added(sel_id_t id);

  /**
   * This signal is emitted when an existing selection has been updated.
   */
  void updated(sel_id_t id);

  /**
   * This signal is emitted when a selection has been removed.
   */
  void removed(sel_id_t id);

  /**
   * The active selection changed.
   */
  void activeChanged(sel_id_t id);

private:
  QMap<sel_id_t, Selection> sels;
  sel_id_t nextid;
  sel_id_t act;
};


#endif/*__DRUMGIZMO_SELECTION_H__*/

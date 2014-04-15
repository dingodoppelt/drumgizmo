/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            selection.cc
 *
 *  Mon Apr 14 10:13:21 CEST 2014
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
#include "selection.h"

Selections::Selections()
  : QObject(NULL)
{
  nextid = 0;
  act = SEL_NONE;
}

sel_id_t Selections::add(Selection selection)
{
  sel_id_t id = nextid++;
  sels[id] = selection;
  added(id);
  return id;
}

Selection Selections::get(sel_id_t id)
{
  if(sels.find(id) == sels.end()) {
    Selection s(0,0,0,0);
    return s;
  }
  return sels[id];
}

QVector<sel_id_t> Selections::ids()
{
  QVector<sel_id_t> v;

  QMap<sel_id_t, Selection>::iterator i = sels.begin();
  while(i != sels.end()) {
    v.push_back(i.key());
    i++;
  }

  return v;
}

void Selections::update(sel_id_t id, Selection selection)
{
  if(sels.find(id) != sels.end()) {
    sels[id] = selection;
    emit updated(id);
  }
}

void Selections::remove(sel_id_t id)
{
  if(sels.find(id) != sels.end()) {
    sels.erase(sels.find(id));
    if(id == act) setActive(SEL_NONE);
    emit removed(id);
  }
}

void Selections::clear()
{
  QVector<sel_id_t> _ids = ids();
  QVector<sel_id_t>::iterator i = _ids.begin();
  while(i != _ids.end()) {
    remove(*i);
    i++;
  }
}

void Selections::setActive(sel_id_t id)
{
  if(sels.find(id) != sels.end()) {
    act = id;
  } else {
    act = SEL_NONE;
  }
  emit activeChanged(act);
}

sel_id_t Selections::active()
{
  return act;
}

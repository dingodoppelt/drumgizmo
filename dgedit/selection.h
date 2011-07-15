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

#include <QMap>

class Selection {
public:
  Selection(int from = 0, int to = 0, int fadein = 0, int fadeout = 0) {
    this->from = from;
    this->to = to;
    this->fadein = fadein;
    this->fadeout = fadeout;
  }
  int from;
  int to;
  int fadein;
  int fadeout;

  QString name;
};

typedef QMap<int, Selection> Selections;

#endif/*__DRUMGIZMO_SELECTION_H__*/

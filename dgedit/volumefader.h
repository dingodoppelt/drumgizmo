/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            volumefader.h
 *
 *  Tue Apr 15 15:46:00 CEST 2014
 *  Copyright 2014 Jonas Suhr Christensen
 *  jsc@umbraculum.org
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
#ifndef __DRUMGIZMO_VOLUMEFADER_H__
#define __DRUMGIZMO_VOLUMEFADER_H__

#include <QWidget>

class VolumeFader : public QWidget {
Q_OBJECT
public:
  VolumeFader();
  ~VolumeFader();

public slots:
  void updatePeakDb(double vol);
  void updatePeakPower(double vol);
  void setVolumeDb(double vol);
  void setVolumePower(double vol);

signals:
  void volumeChangedDb(double vol);
  void volumeChangedPower(double vol);
};

#endif/*__DRUMGIZMO_VOLUMEFADER_H__*/

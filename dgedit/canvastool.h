/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            canvastool.h
 *
 *  Thu Jul 28 20:12:25 CEST 2011
 *  Copyright 2011 Bent Bisballe Nyeng
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
#ifndef __DRUMGIZMO_CANVASTOOL_H__
#define __DRUMGIZMO_CANVASTOOL_H__

class CanvasTool : public QObject {
Q_OBJECT
public:
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void resizeEvent(QResizeEvent *event);
  virtual void paintEvent(QPaintEvent *event);
  virtual void keyReleaseEvent(QKeyEvent *event);

signals:
  void activateChanged(bool activestate);

public slots:
  void setActive(bool active)
  {
    _active = active;
    emit activateChanged(active);
  }
  
  void activate()
  {
    setActive(true);
  }

  void disactivate()
  {
    setActive(false);
  }

  bool isActive()
  {
    return _active;
  }

private:
  bool _active;
};

#endif/*__DRUMGIZMO_CANVASTOOL_H__*/

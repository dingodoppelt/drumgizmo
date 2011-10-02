/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            canvastool.cc
 *
 *  Thu Jul 28 23:01:36 CEST 2011
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
#include "canvastool.h"

#include <stdio.h>

bool CanvasTool::mouseMoveEvent(QMouseEvent *)
{
  return false;
}

bool CanvasTool::mousePressEvent(QMouseEvent *)
{
  return false;
}

bool CanvasTool::mouseReleaseEvent(QMouseEvent *)
{
  return false;
}

void CanvasTool::resizeEvent(QResizeEvent *)
{
}

void CanvasTool::paintEvent(QPaintEvent *, QPainter &)
{
}

void CanvasTool::keyReleaseEvent(QKeyEvent *)
{
}

void CanvasTool::setActive(bool active)
{
  printf("setActive(%d)\n", active);
  _active = active;
  emit activateChanged(active);
}
  
void CanvasTool::activate()
{
  setActive(true);
}

void CanvasTool::disactivate()
{
  setActive(false);
}

bool CanvasTool::isActive()
{
  return _active;
}

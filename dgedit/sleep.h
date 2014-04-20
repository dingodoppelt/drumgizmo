/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            sleep.h
 *
 *  Sun Apr 20 18:54:58 CEST 2014
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
#ifndef __DRUMGIZMO_SLEEP_H__
#define __DRUMGIZMO_SLEEP_H__

#include <QThread>

class __Sleeper : public QThread {
public:
  void msleep(unsigned long msecs) { QThread::msleep(msecs); }
  void sleep(unsigned long secs) { QThread::sleep(secs); }
  void usleep(unsigned long usecs) { QThread::usleep(usecs); }
};

inline void q_msleep(unsigned long msecs)
{
  __Sleeper s;
  s.msleep(msecs);
}

inline void q_sleep(unsigned long secs)
{
  __Sleeper s;
  s.sleep(secs);
}

inline void q_usleep(unsigned long usecs)
{
  __Sleeper s;
  s.usleep(usecs);
}

#endif/*__DRUMGIZMO_SLEEP_H__*/

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            semaphore.h
 *
 *  Sat Oct  8 17:44:13 CEST 2005
 *  Copyright  2005 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
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
#ifndef __PRACRO_SEMAPHORE_H__
#define __PRACRO_SEMAPHORE_H__

struct semaphore_private_t;

class Semaphore {
public:
  Semaphore(const char *name = "");
  ~Semaphore();

  void post();
  void wait();

private:
	struct semaphore_private_t *prv{nullptr};
	const char *name{nullptr};
};

#endif/*__PRACRO_SEMAPHORE_H__*/

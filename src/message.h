/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            message.h
 *
 *  Wed Mar 20 15:50:57 CET 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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
#ifndef __DRUMGIZMO_MESSAGE_H__
#define __DRUMGIZMO_MESSAGE_H__

class Message {
public:
  typedef enum {
    LoadStatus,
  } type_t;
  virtual ~Message() {}
  virtual type_t type() = 0;
};

class LoadStatus : public Message {
public:
  type_t type() { return Message::LoadStatus; }
  unsigned int number_of_files;
  unsigned int numer_of_files_loaded;
  std::string current_file;
};

#endif/*__DRUMGIZMO_MESSAGE_H__*/

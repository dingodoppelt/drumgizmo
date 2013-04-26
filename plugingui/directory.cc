/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            directory.cc
 *
 *  Tue Apr 23 22:01:07 CEST 2013
 *  Copyright 2013 Jonas Suhr Christensen
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
#include "directory.h"

#include <dirent.h>

#include <hugin.hpp>

Directory::Directory(std::string path) {
  setPath(path);
}

Directory::~Directory() {
}

void Directory::setPath(std::string path) {
  DEBUG(directory, "Setting path to '%s'\n", path.c_str());
  this->_path = path;
  refresh();
}

size_t Directory::count() {
  return _files.size();
}

void Directory::refresh() {
  _files = listFiles(this->_path);
}

bool Directory::cd(std::string dir) {
  DEBUG(directory, "Changing to '%s'\n", dir.c_str());
  int r = chdir(dir.c_str());
  refresh();
  if(!r) return true;
  else return false;
}

bool Directory::cdUp() {
  return this->cd("..");
}

std::string Directory::path() {
  return _path;
}

Directory::EntryList Directory::entryList() {
  return _files;
}

#define MAX_FILE_LENGTH 1024
std::string Directory::cwd() {
  char path[MAX_FILE_LENGTH];
  char* c = getcwd(path, MAX_FILE_LENGTH);

  if(c) return c;
  else return "";
}

std::string Directory::cleanPath(std::string path) {
  // TODO!
  return path;
}

Directory::EntryList Directory::listFiles(std::string path) {
  Directory::EntryList entries;
  DIR *dir = opendir(".");
  if(!dir) {
    return entries;
  }

  struct dirent *entry;
  while((entry = readdir(dir)) != NULL) {
    entries.push_back(entry->d_name);
  }

  return entries;
}

bool Directory::isRoot(std::string path) {
  if(path == "/") return true;
  else return false;
}

Directory::DriveList Directory::drives() {
  Directory::DriveList drives;
#ifdef WIN32
  unsigned int d = GetLogicalDrives();
  for(int i = 0; i < 32; i++) {
    if(d & (1 << i)) {
      drive_t drive;
      char name[] = "X:";
      name[0] = i + 'A';

      char num[32];
      sprintf(num, "%d", i);

      drive.name = name;
      drive.number = num;
      drives.push_back(drive);
    }
  }
#endif
  return drives;
}

bool Directory::isDir()
{
  return isDir(path());
}

bool Directory::exists(std::string filename) {
  return !isDir(path() + "/" + filename);
}

bool Directory::isDir(std::string path) {
  DEBUG(directory, "Is '%s' dir?\n", path.c_str());
  struct stat st;
  if(stat(path.c_str(), &st) == 0) {
    if((st.st_mode & S_IFDIR) != 0) {
      DEBUG(directory, "Yes\n");
      return true;
    }
  }
  DEBUG(directory, "No\n");
  return false;
}

#ifdef TEST_DIRECTORY
//Additional dependency files
//deps:
//Required cflags (autoconf vars may be used)
//cflags:
//Required link options (autoconf vars may be used)
//libs:
#include "test.h"

TEST_BEGIN;

// TODO: Put some testcode here (see test.h for usable macros).

TEST_END;

#endif/*TEST_DIRECTORY*/

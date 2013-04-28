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
#include <stdio.h>

#ifdef WIN32
#include <direct.h>
#include <windows.h>
#endif

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
  _files = listFiles(_path);
}

bool Directory::cd(std::string dir) {
  //TODO: Should this return true or false?
  if(dir.empty() || dir == ".") return true;

  DEBUG(directory, "Changing to '%s'\n", dir.c_str());
  if(exists(_path + "/" + dir)) {
    std::string path = _path + "/" + dir;
    setPath(path);
    refresh();
    return true;
  }
  else return false;
}

bool Directory::cdUp() {
  return this->cd("..");
}

std::string Directory::path() {
  setPath(cleanPath(_path));
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
  WARN(directory, "Cleaning path '%s'\n", path.c_str());

  Directory::Path pathlst = parsePath(path);
  return Directory::pathToStr(pathlst);  
}

Directory::EntryList Directory::listFiles(std::string path) {
  DEBUG(directory, "Listing files in '%s'\n", path.c_str());

  Directory::EntryList entries;
  DIR *dir = opendir(path.c_str());
  if(!dir) {
    DEBUG(directory, "Couldn't open directory '%s\n", path.c_str()); 
    return entries;
  }

  struct dirent *entry;
  while((entry = readdir(dir)) != NULL) {
    std::string name = entry->d_name;
    if(name == ".") continue;
    if(Directory::isRoot(path) && name == "..") continue;
    entries.push_back(entry->d_name);
  }

  return entries;
}

bool Directory::isRoot(std::string path) {
  //TODO: Handle WIN32

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

      drive.name = name;
      drive.number = i;
      drives.push_back(drive);
    }
  }
#endif
  return drives;
}

bool Directory::isDir()
{
  return isDir(_path);
}

bool Directory::fileExists(std::string filename) {
  return !isDir(_path + "/" + filename);
}

bool Directory::exists(std::string path) {
  struct stat st;
  if(stat(path.c_str(), &st) == 0) {
    return true;
  }
  else return false;
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

Directory::Path Directory::parsePath(std::string path_str) {
  //TODO: Handle "." input and propably other special cases

  DEBUG(directory, "Parsing path '%s'", path_str.c_str());
  Directory::Path path;
  
  std::string current_char;
  std::string prev_char;
  std::string dir;
  for(size_t c = 0; c < path_str.size(); c++) {
    current_char = path_str.at(c);

    if(current_char == "/") {
      if(prev_char == "/") {
        dir.clear();
        prev_char = current_char;
        continue;
      }
      else if(prev_char == ".") {
        prev_char = current_char;
        continue;
      }
      
      if(!dir.empty()) path.push_back(dir);
      dir.clear();
      continue;
    }
    else if(current_char == ".") {
      if(prev_char == ".") {
        dir.clear();
        if(!path.empty()) path.pop_back();
        continue;
      }
    }
    
    dir += current_char;
    prev_char = current_char;
  }

  if(!dir.empty()) path.push_back(dir);

  return path;
}

std::string Directory::pathToStr(Directory::Path& path) {
  std::string cleaned_path;
  DEBUG(directory, "Number of directories in path %d\n", path.size());

  for(Directory::Path::iterator it = path.begin();
      it != path.end(); it++) {
    std::string dir = *it;
    DEBUG(directory, "\tDir '%s'\n", dir.c_str());
    cleaned_path += "/" + dir;
  }

  DEBUG(directory, "Cleaned path '%s'\n", cleaned_path.c_str());

  if(cleaned_path.empty()) cleaned_path = "/";

  return cleaned_path; 
}

std::string Directory::pathDirectory(std::string filepath) {
  if(Directory::isDir(filepath)) return filepath;

  Directory::Path path = parsePath(filepath);
  if(path.size() > 0) path.pop_back();

  return Directory::pathToStr(path);
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

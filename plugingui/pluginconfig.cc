/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            config.cc
 *
 *  Tue Jun  3 13:54:05 CEST 2014
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
#include "pluginconfig.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "directory.h"

#ifdef WIN32
#include <direct.h>
#include <windows.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#else
#endif

#include <hugin.hpp>
  
#define CONFIGFILENAME "plugingui.conf"

#ifdef WIN32
  #define SEP "\\"
  #define CONFIGDIRNAME "drumgizmo"
#else
  #define SEP "/"
  #define CONFIGDIRNAME ".drumgizmo"
#endif

Config::Config()
{

}

Config::~Config()
{

}

FILE* openFilePtr(std::string mode) {
#ifdef WIN32
  std::string configpath = ".";
  TCHAR szPath[256];
  if(SUCCEEDED(SHGetFolderPath(NULL,
                               CSIDL_APPDATA | CSIDL_FLAG_CREATE,
                               NULL,
                               0,
                               szPath))); {
    DEBUG(config, "WINDOWS APP DATA PATH:%s\n", szPath);
    PathAppend(szPath, drumgizmo);
//    if(!Directory::exists(configpath)) {
//      CreateDirectory(szPath, NULL); 
//    }
  }
#else
  std::string configpath = strdup(getenv("HOME"));
  configpath += SEP;
  configpath += CONFIGDIRNAME;
  if(!Directory::exists(configpath)) {
    if( (mkdir(configpath.c_str(), 0755)) < 0) return NULL;
  }
#endif

  FILE *fp;
  std::string configfile = configpath;
  configfile += SEP;
  configfile += CONFIGFILENAME;
  if(! (fp = fopen(configfile.c_str(), mode.c_str())) ) {
    return NULL;
  }

  return fp;
}

void Config::load()
{
  FILE *fp = openFilePtr("r");
  if(!fp) return;

  lastkit.clear();
  lastmidimap.clear();

  char buf[4096];
  while( fgets(buf, 4096, fp) ) {
    if(!strncmp(buf, "lastkit:", 8)) {
      // Dont copy newline
      if(strlen(buf) > 8 + 1) lastkit.append(buf+8, strlen(buf+8) - 1);
    }
    if(!strncmp(buf, "lastmidimap:", 12)) {
      // Dont copy newline
      if(strlen(buf) > 12+1) lastmidimap.append(buf+12, strlen(buf+12) - 1);
    }
  }
}

void Config::save()
{
  FILE *fp = openFilePtr("w");
  if(!fp) return;

  std::string buf;
  buf.append("lastkit:" + lastkit + '\n');
  buf.append("lastmidimap:" + lastmidimap + '\n');

  fputs(buf.c_str(), fp);

  fclose(fp);
}

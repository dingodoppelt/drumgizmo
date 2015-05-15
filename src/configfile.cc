/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            configfile.cc
 *
 *  Thu May 14 14:51:39 CEST 2015
 *  Copyright 2015 Bent Bisballe Nyeng
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
#include "configfile.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifdef WIN32
#include <direct.h>
#include <windows.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#else
#endif

#include <hugin.hpp>
  
#ifdef WIN32
  #define SEP "\\"
  #define CONFIGDIRNAME ".drumgizmo"
#else
  #define SEP "/"
  #define CONFIGDIRNAME ".drumgizmo"
#endif

/**
 * Return the path containing the config files.
 */
static std::string configPath()
{
#ifdef WIN32
  std::string configpath;
  TCHAR szPath[256];
  if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE,
                               NULL, 0, szPath))) {
    configpath = szPath;
  }
#else
  std::string configpath = strdup(getenv("HOME"));
#endif
  configpath += SEP;
  configpath += CONFIGDIRNAME;

  return configpath;
}

/**
 * Calling this makes sure that the config path exists
 */
static bool createConfigPath()
{
  std::string configpath = configPath();

  struct stat st;
  if(stat(configpath.c_str(), &st) == 0) {
    DEBUG(configfile, "No configuration exists, creating directory '%s'\n",
          configpath.c_str());
#ifdef WIN32
    if(mkdir(configpath.c_str()) < 0) { 
#else
    if(mkdir(configpath.c_str(), 0755) < 0) { 
#endif
      DEBUG(pluginconfig, "Could not create config directory\n");
    }

    return false;
  }

  return true;
}

ConfigFile::ConfigFile(std::string filename)
  : filename(filename)
  , fp(NULL)
{
}

ConfigFile::~ConfigFile()
{
}

void ConfigFile::load()
{
  DEBUG(pluginconfig, "Loading config file...\n");
  if(!open("r")) return;

  values.clear();

  std::string line;
  while(true) {
    line = readLine();

    if(line == "") break;

    if(line[line.size() - 1] == '\n') {
      line = line.substr(0, line.size() - 1); // strip ending newline.
    }

    std::size_t colon = line.find(':');

    if(colon == std::string::npos) break; // malformed line

    std::string key = line.substr(0, colon);
    std::string value = line.substr(colon + 1);

    printf("key['%s'] value['%s']\n", key.c_str(), value.c_str());

    if(key != "") {
      values[key] = value;
    }
  }

  close();
}

void ConfigFile::save()
{
  DEBUG(pluginconfig, "Saving configuration...\n");

  createConfigPath();

  if(!open("w")) return;

  std::map<std::string, std::string>::iterator v = values.begin();
  for(; v != values.end(); ++v) {
    fprintf(fp, "%s:%s\n", v->first.c_str(), v->second.c_str());
  }

  close();
}

std::string ConfigFile::getValue(const std::string& key)
{
  if(values.find(key) != values.end()) {
    return values[key];
  }

  return "";
}

void ConfigFile::setValue(const std::string& key, const std::string& value)
{
  values[key] = value;
}

bool ConfigFile::open(std::string mode)
{
  if(fp) close();

  std::string configpath = configPath();

  std::string configfile = configpath;
  configfile += SEP;
  configfile += filename;

  DEBUG(pluginconfig, "Opening config file '%s'\n", configfile.c_str());
  fp = fopen(configfile.c_str(), mode.c_str());

  if(!fp) return false;

  return true;
}

void ConfigFile::close()
{
  fclose(fp);
  fp = NULL;
}

std::string ConfigFile::readLine()
{
  if(!fp) return "";

  std::string line;

  char buf[1024];
  while(!feof(fp)) {
    char *s = fgets(buf, sizeof(buf), fp);
    if(s) {
      line += buf;
      if(buf[strlen(buf) - 1] == '\n') break;
    }
  }

  return line;
}

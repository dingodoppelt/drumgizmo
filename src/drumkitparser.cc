/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumkitparser.cc
 *
 *  Tue Jul 22 16:24:59 CEST 2008
 *  Copyright 2008 Bent Bisballe Nyeng
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
#include "drumkitparser.h"

#include <string.h>
#include <stdio.h>

#define DIR_SEPERATOR '/'

DrumKitParser::DrumKitParser(const std::string &kitfile, DrumKit &k)
  : kit(k)
{
  fd = fopen(kitfile.c_str(), "r");
  if(!fd) return;
}

DrumKitParser::~DrumKitParser()
{
  if(fd) fclose(fd);
}

void DrumKitParser::startTag(std::string name,
                             std::map< std::string, std::string> attributes)
{
  if(name == "drumkit") {
    kit.name = attributes["name"];
    kit.description = attributes["description"];
  }

  if(name == "channels") {}

  if(name == "channel") {
    //    Channel *c = new Channel(attributes["name"]);
    //    dk->channels[attributes["name"]] = c;
  }

  if(name == "instruments") {
  }

  if(name == "instrument") {
  }

  if(name == "channelmap") {
  }

  if(name == "midimaps") {
  }

  if(name == "midimap") {
  }
}

void DrumKitParser::endTag(std::string name)
{
}

int DrumKitParser::readData(char *data, size_t size)
{
  if(!fd) return -1;
  return fread(data, 1, size, fd);
}


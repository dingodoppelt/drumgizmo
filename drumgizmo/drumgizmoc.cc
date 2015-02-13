/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            cli.cc
 *
 *  Thu Sep 16 10:23:22 CEST 2010
 *  Copyright 2010 Bent Bisballe Nyeng
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
#include <config.h>
#include <getopt.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "drumgizmo.h"

#include "audiooutputenginedl.h"
#include "audioinputenginedl.h"

#include "event.h"

static const char version_str[] =
"DrumGizmo v" VERSION "\n"
;

static const char copyright_str[] =
"Copyright (C) 2008-2011 Bent Bisballe Nyeng - Aasimon.org.\n"
"This is free software.  You may redistribute copies of it under the terms of\n"
"the GNU General Public License <http://www.gnu.org/licenses/gpl.html>.\n"
"There is NO WARRANTY, to the extent permitted by law.\n"
"\n"
"Written by Bent Bisballe Nyeng (deva@aasimon.org)\n"
;

static const char usage_str[] =
"Usage: %s [options] drumkitfile\n"
"Options:\n"
"  -p, --preload          Load entire kit audio files into memory (uses ALOT of memory).\n"
"  -i, --inputengine dummy|test|jackmidi|midifile  Use said event input engine.\n"
"  -I, --inputparms parmlist  Set input engine parameters.\n"
"  -o, --outputengine dummy|alsa|jackaudio|wavfile  Use said audio output engine.\n"
"  -O, --outputparms parmlist  Set output engine parameters.\n"
"  -e, --endpos Number of samples to process, -1: infinite.\n"
"  -v, --version          Print version information and exit.\n"
"  -h, --help             Print this message and exit.\n"
;

int main(int argc, char *argv[])
{
  int c;

  std::string outputengine;
  std::string inputengine;
  std::string iparms;
  std::string oparms;
  bool preload = false;
  int endpos = -1;

  int option_index = 0;
  while(1) {
    static struct option long_options[] = {
      {"preload", no_argument, 0, 'p'},
      {"inputengine", required_argument, 0, 'i'},
      {"inputparms", required_argument, 0, 'I'},
      {"outputengine", required_argument, 0, 'o'},
      {"outputparms", required_argument, 0, 'O'},
      {"endpos", required_argument, 0, 'e'},
      {"version", no_argument, 0, 'v'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}
    };
    
    c = getopt_long (argc, argv, "hvpo:O:i:I:e:", long_options, &option_index);
    
    if (c == -1)
      break;

    switch(c) {
    case 'i':
      inputengine = optarg;
      if(inputengine == "help") {
        printf("Available input engines: jackmidi, midifile.\n");
        return 0;
      }
      break;

    case 'I':
      iparms = optarg;
      break;

    case 'o':
      outputengine = optarg;
      if(outputengine == "help") {
        printf("Available output engines: alsa, jackaudio, wavfile.\n");
        return 0;
      }
      break;

    case 'O':
      oparms = optarg;
      break;

    case 'p':
      preload = true;
      break;
 
    case 'e':
      endpos = atoi(optarg);
      break;

    case '?':
    case 'h':
      printf("%s", version_str);
      printf(usage_str, argv[0]);
      return 0;

    case 'v':
      printf("%s", version_str);
      printf("%s", copyright_str);
      return 0;

    default:
      break;
    }
  }

  if(inputengine == "") {
    printf("Missing input engine\n");
    return 1;
  }

  AudioInputEngine *ie = new AudioInputEngineDL(inputengine);

  if(ie == NULL) {
    printf("Invalid input engine: %s\n", inputengine.c_str());
    return 1;
  }

  {
  std::string parm;
  std::string val;
  bool inval = false;
  for(size_t i = 0; i < iparms.size(); i++) {
    if(iparms[i] == ',') {
      ie->setParm(parm, val);
      parm = "";
      val = "";
      inval = false;
      continue;
    }

    if(iparms[i] == '=') {
      inval = true;
      continue;
    }

    if(inval) {
      val += iparms[i];
    } else {
      parm += iparms[i];
    }
  }
  if(parm != "") ie->setParm(parm, val);
  }

  if(outputengine == "") {
    printf("Missing output engine\n");
    return 1;
  }

  AudioOutputEngineDL *oe = new AudioOutputEngineDL(outputengine);

  if(oe == NULL) {
    printf("Invalid output engine: %s\n", outputengine.c_str());
    return 1;
  }

  {
  std::string parm;
  std::string val;
  bool inval = false;
  for(size_t i = 0; i < oparms.size(); i++) {
    if(oparms[i] == ',') {
      oe->setParm(parm, val);
      parm = "";
      val = "";
      inval = false;
      continue;
    }

    if(oparms[i] == '=') {
      inval = true;
      continue;
    }

    if(inval) {
      val += oparms[i];
    } else {
      parm += oparms[i];
    }
  }
  if(parm != "") oe->setParm(parm, val);
  }

  std::string kitfile;

  if(option_index < argc) {
    printf("non-option ARGV-elements: ");
    while (optind < argc) {
      if(kitfile != "") {
        fprintf(stderr, "Can only handle a single kitfile.\n");
        printf(usage_str, argv[0]);
        return 1;
      }
      kitfile = argv[optind++];
    }
    printf("\n");
  } else {
    fprintf(stderr, "Missing kitfile.\n");
    printf(usage_str, argv[0]);
    return 1;
  }
  
  printf("Using kitfile: %s\n", kitfile.c_str());

  DrumGizmo gizmo(oe, ie);
  if(kitfile == "" || !gizmo.loadkit(kitfile)) {
    printf("Failed to load \"%s\".\n", kitfile.c_str());
    return 1;
  }

  gizmo.setSamplerate(oe->samplerate());

  if(!gizmo.init(preload)) {
    printf("Failed init engine.\n");
    return 1;
  }

  gizmo.run(endpos);

  printf("Quit.\n"); fflush(stdout);

  delete oe;
  delete ie;

  return 0;
}

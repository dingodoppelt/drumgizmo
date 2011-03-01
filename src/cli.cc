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

#include "audiooutputengine.h"
#include "audioinputengine.h"

#include "event.h"

static const char version_str[] =
"DrumGizmo v" VERSION "\n"
;

static const char copyright_str[] =
"Copyright (C) 2008-2009 Bent Bisballe Nyeng - Aasimon.org.\n"
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
"  -o, --outputengine alsa|jack|sndfile  Use said audio engine.\n"
"  -i, --inputengine jackmidi|midifile  Use said audio engine.\n"
"  -v, --version          Print version information and exit.\n"
"  -h, --help             Print this message and exit.\n"
;

int main(int argc, char *argv[])
{
  int c;

  std::string outputengine;
  std::string inputengine;
  bool preload = false;

  int option_index = 0;
  while(1) {
    static struct option long_options[] = {
      {"preload", no_argument, 0, 'p'},
      {"outputengine", required_argument, 0, 'o'},
      {"inputengine", required_argument, 0, 'i'},
      {"help", no_argument, 0, 'h'},
      {"version", no_argument, 0, 'v'},
      {0, 0, 0, 0}
    };
    
    c = getopt_long (argc, argv, "hvpo:i:", long_options, &option_index);
    
    if (c == -1)
      break;

    switch(c) {
    case 'o':
      outputengine = optarg;
      break;

    case 'i':
      inputengine = optarg;
      break;

    case 'p':
      preload = true;
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

  if(outputengine == "") {
    printf("Missing output engine\n");
    return 1;
  }

  AudioOutputEngine *oe = createAudioOutputEngine(outputengine);

  if(oe == NULL) {
    printf("Invalid output engine: %s\n", outputengine.c_str());
    return 1;
  }


  if(inputengine == "") {
    printf("Missing input engine\n");
    return 1;
  }

  AudioInputEngine *ie = createAudioInputEngine(inputengine);

  if(ie == NULL) {
    printf("Invalid input engine: %s\n", outputengine.c_str());
    return 1;
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

  Channels channels;
  Channel c1; c1.num = 0;
  Channel c2; c2.num = 1;
  channels.push_back(c1);
  channels.push_back(c2);
  ChannelMixer m(channels, &channels[0]);

  DrumGizmo gizmo(*oe, *ie, m);
  if(/*kitfile == "" ||*/ !gizmo.loadkit(kitfile)) {
    printf("Failed to load \"%s\".\n", kitfile.c_str());
    return 1;
  }

  if(!gizmo.init(preload)) {
    printf("Failed init drumkit.\n");
    return 1;
  }

  gizmo.run();

  printf("Quit.\n"); fflush(stdout);

  delete oe;
  delete ie;

  return 0;
}

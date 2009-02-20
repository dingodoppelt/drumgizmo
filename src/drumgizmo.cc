/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumgizmo.cc
 *
 *  Sun Jul 20 19:25:01 CEST 2008
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
#include <config.h>
#include <getopt.h>
#include "jackclient.h"
#include "drumkitparser.h"
#include "midiplayer.h"

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
"  -P, --preload_vel vel  Load all files with velocity above vel into memory (uses a little less memory).\n"
"  -m, --midi midifile    Load midifile, and play it.\n"
"  -v, --version          Print version information and exit.\n"
"  -h, --help             Print this message and exit.\n"
;

int main(int argc, char *argv[])
{
  int c;

  char *midifile = NULL;
  bool preload = true;
  int min_velocity = 18;

  int option_index = 0;
  while(1) {
    static struct option long_options[] = {
      {"preload_vel", required_argument, 0, 'P'},
      {"preload", no_argument, 0, 'p'},
      {"midi", required_argument, 0, 'm'},
      {"help", no_argument, 0, 'h'},
      {"version", no_argument, 0, 'v'},
      {0, 0, 0, 0}
    };
    
    c = getopt_long (argc, argv, "hvpP:m:", long_options, &option_index);
    
    if (c == -1)
      break;

    switch(c) {
    case 'm':
      midifile = strdup(optarg);
      break;

    case 'p':
      preload = true;
      break;
 
    case 'P':
      preload = true;
      min_velocity = atoi(optarg);
      break;

    case '?':
    case 'h':
      printf(version_str);
      printf(usage_str, argv[0]);
      return 0;

    case 'v':
      printf(version_str);
      printf(copyright_str);
      return 0;

    default:
      break;
    }
  }

  if(argc < option_index + 2) {
    fprintf(stderr, "Missing kitfile.\n");
    printf(usage_str, argv[0]);
    return 1;
  }

  if(argc > option_index + 2) {
    fprintf(stderr, "Can only handle a single kitfile.\n");
    printf(usage_str, argv[0]);
    return 1;
  }

  std::string kitfile = argv[option_index + 1];
  
  printf("Using kitfile: %s\n", kitfile.c_str());

  DrumKitParser parser(kitfile, preload, min_velocity);
  if(parser.parse()) return 1;

  JackClient client(parser.getDrumkit());

  client.activate();

  if(midifile) MidiPlayer player(midifile);

  while(1) sleep(1);

  return 0;
}

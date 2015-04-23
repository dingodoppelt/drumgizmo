/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            cachemanagertest.cc
 *
 *  Sun Apr 19 10:15:59 CEST 2015
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
#include <cppunit/extensions/HelperMacros.h>

#include <cachemanager.h>
#include <unistd.h>

class test_cachemanager : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(test_cachemanager);
  CPPUNIT_TEST(singlechannel_nonthreaded);
	CPPUNIT_TEST(singlechannel_threaded);
  CPPUNIT_TEST(multichannel_nonthreaded);
  CPPUNIT_TEST(multichannel_threaded);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {}
	void tearDown() {}

  void testit(const char *filename, int channel, bool threaded)
  {

    // Reference file:
    AudioFile afref(filename, channel);
    printf("afref.load\n");
    afref.load(ALL_SAMPLES);

    // Input file:
    AudioFile af(filename, channel);
    printf("af.load\n");
    af.load(ALL_SAMPLES);
    //af.load(PRELOADSIZE);

    CacheManager cm;
    printf("cm.init\n");
    cm.init(100, threaded);

    cacheid_t id;
    // TODO: test 0 ... FRAMESIZE - 1
    size_t initial_samples_needed = (FRAMESIZE - 1) / 2;

    printf("open\n");
    sample_t *s = cm.open(&af, initial_samples_needed, channel, id);
    size_t size = initial_samples_needed;
    size_t offset = 0;

    // Test pre cache:
    for(size_t i = 0; i < size; i++) {
      CPPUNIT_ASSERT_EQUAL(afref.data[offset], s[i]);
      offset++;
    }

    // Test the rest
    while(offset < afref.size) {

      if(threaded) {
        usleep(1000000.0 / 44100.0 * FRAMESIZE);
        //sleep(1); // sleep 1 second
      }

      //printf("offset: %d\t", offset);
      s = cm.next(id, size);
      //printf("next -> size: %d\n", size);
      for(size_t i = 0; i < size && (offset < afref.size); i++) {
        CPPUNIT_ASSERT_EQUAL(afref.data[offset], s[i]);
        offset++;
      }
    }

    printf("done\n");
	}

  void singlechannel_nonthreaded()
  {
    const char filename[] = "kit/ride-single-channel.wav";
    int channel = 0;
    bool threaded = false;
    testit(filename, channel, threaded);
  }

  void singlechannel_threaded()
  {
    const char filename[] = "kit/ride-single-channel.wav";
    int channel = 0;
    bool threaded = true;
    testit(filename, channel, threaded);
  }

  void multichannel_nonthreaded()
  {
    const char filename[] = "kit/ride-multi-channel.wav";
    int channel = 0;
    bool threaded = false;
    testit(filename, channel, threaded);
  }

  void multichannel_threaded()
  {
    const char filename[] = "kit/ride-multi-channel.wav";
    int channel = 0;
    bool threaded = true;
    testit(filename, channel, threaded);
  }

};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(test_cachemanager);




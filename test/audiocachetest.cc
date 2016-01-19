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

#include <audiofile.h>
#include <audiocache.h>
#include <unistd.h>

#define FRAMESIZE 64

class AudioCacheTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(AudioCacheTest);
	CPPUNIT_TEST(singleChannelNonThreaded);
	CPPUNIT_TEST(singleChannelThreaded);
	CPPUNIT_TEST(multiChannelNonThreaded);
	CPPUNIT_TEST(multiChannelThreaded);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {}
	void tearDown() {}

	//! Test runner.
	//! \param filename The name of the file to read.
	//! \param channel The channel number to do comparison on.
	//! \param thread Control if this test is running in threaded mode or not.
	//! \param framesize The initial framesize to use.
	void testHelper(const char* filename, int channel, bool threaded,
	                int framesize)
	{
		// Reference file:
		AudioFile audioFileRef(filename, channel);
		printf("audioFileRef.load\n");
		audioFileRef.load(ALL_SAMPLES);

		// Input file:
		AudioFile audioFile(filename, channel);
		printf("audioFile.load\n");
		audioFile.load(4096);

		AudioCache audioCache;
		printf("audioCache.init\n");
		audioCache.init(100);
		audioCache.setAsyncMode(threaded);

		// Set initial (upper limit) framesize
		audioCache.setFrameSize(framesize);

		cacheid_t id;

		for(size_t initial_samples_needed = 0;
		    initial_samples_needed < (framesize + 1); ++initial_samples_needed)
		{

			printf("open: initial_samples_needed: %d\n", initial_samples_needed);
			sample_t *samples =
				audioCache.open(&audioFile, initial_samples_needed, channel, id);
			size_t size = initial_samples_needed;
			size_t offset = 0;

			// Test pre cache:
			for(size_t i = 0; i < size; ++i)
			{
				CPPUNIT_ASSERT_EQUAL(audioFileRef.data[offset], samples[i]);
				++offset;
			}

			// Test the rest
			while(offset < audioFileRef.size)
			{
				if(threaded)
				{
					// Wait until we are finished reading
					int timeout = 1000;
					while(!audioCache.isReady(id))
					{
						usleep(1000);
						if(--timeout == 0)
						{
							CPPUNIT_ASSERT(false); // timeout
						}
					}
				}

				samples = audioCache.next(id, size);

				CPPUNIT_ASSERT_EQUAL(0, (int)audioCache.getNumberOfUnderruns());

				for(size_t i = 0; (i < size) && (offset < audioFileRef.size); ++i)
				{
					if(audioFileRef.data[offset] != samples[i])
					{
						printf("-----> offset: %d, size: %d, diff: %d,"
						       " i: %d, size: %d, block-diff: %d\n",
						       offset, audioFileRef.size, audioFileRef.size - offset,
						       i, size, size - i);
					}
					CPPUNIT_ASSERT_EQUAL(audioFileRef.data[offset], samples[i]);
					++offset;
				}
			}

			audioCache.close(id);
		}

		printf("done\n");
	}

	void singleChannelNonThreaded()
	{
		printf("\nsinglechannel_nonthreaded()\n");
		const char filename[] = "kit/ride-single-channel.wav";
		int channel = 0;
		bool threaded = false;
		testHelper(filename, channel, threaded, FRAMESIZE);
	}

	void singleChannelThreaded()
	{
		printf("\nsinglechannel_threaded()\n");
		const char filename[] = "kit/ride-single-channel.wav";
		int channel = 0;
		bool threaded = true;
		testHelper(filename, channel, threaded, FRAMESIZE);
	}

	void multiChannelNonThreaded()
	{
		printf("\nmultichannel_nonthreaded()\n");
		const char filename[] = "kit/ride-multi-channel.wav";
		int channel = 0;
		bool threaded = false;
		testHelper(filename, channel, threaded, FRAMESIZE);
		++channel;
		testHelper(filename, channel, threaded, FRAMESIZE);
	}

	void multiChannelThreaded()
	{
		printf("\nmultichannel_threaded()\n");
		const char filename[] = "kit/ride-multi-channel.wav";
		int channel = 0;
		bool threaded = true;
		testHelper(filename, channel, threaded, FRAMESIZE);
		++channel;
		testHelper(filename, channel, threaded, FRAMESIZE);
	}

};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(AudioCacheTest);

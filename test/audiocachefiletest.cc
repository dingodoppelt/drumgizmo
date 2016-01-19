/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            audiocachefiletest.cc
 *
 *  Thu Jan  7 15:43:12 CET 2016
 *  Copyright 2016 Bent Bisballe Nyeng
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

#include <cstring>

#include <audiocachefile.h>
#include <audiofile.h>

class TestableAudioCacheFiles : public AudioCacheFiles
{
public:
	//CacheAudioFile& getAudioFile(const std::string& filename);
	//void release(const std::string& filename);
	int getRef(const std::string& filename)
	{
		if(audiofiles.find(filename) == audiofiles.end())
		{
			return -1;
		}

		return audiofiles[filename]->ref;
	}
};

class AudioCacheFileTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(AudioCacheFileTest);
	CPPUNIT_TEST(refTest);
	CPPUNIT_TEST(readTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {}
	void tearDown() {}

	void refTest()
	{
		TestableAudioCacheFiles audiofiles;
		std::string filename = "kit/ride-single-channel.wav";
		CPPUNIT_ASSERT_EQUAL(-1, audiofiles.getRef(filename));

		audiofiles.getFile(filename);
		CPPUNIT_ASSERT_EQUAL(1, audiofiles.getRef(filename));

		audiofiles.getFile(filename);
		CPPUNIT_ASSERT_EQUAL(2, audiofiles.getRef(filename));

		audiofiles.releaseFile(filename);
		CPPUNIT_ASSERT_EQUAL(1, audiofiles.getRef(filename));

		audiofiles.releaseFile(filename);
		CPPUNIT_ASSERT_EQUAL(-1, audiofiles.getRef(filename));
	}

	void readTestHelper(size_t bufferSize)
	{
		printf("Test buffer size: %d samples\n", bufferSize);

		std::string filename = "kit/ride-multi-channel.wav";
		AudioFile* refFile[13];
		for(size_t c = 0; c < 13; ++c)
		{
			refFile[c] = new AudioFile(filename, c);
			refFile[c]->load();
		}

		AudioCacheFile file(filename);
		CPPUNIT_ASSERT_EQUAL(filename, file.getFilename());
		CPPUNIT_ASSERT_EQUAL(13, (int)file.getChannelCount()); // Sanity check

		CacheChannels channels;

		sample_t samples[13][bufferSize];
		volatile bool ready[13];
		for(size_t c = 0; c < 13; ++c)
		{
			for(int c = 0; c < 13; ++c)
			{
				for(int i = 0; i < bufferSize; ++i)
				{
					samples[c][i] = 42;
				}
			}

			channels.push_back(
				{
					c, // channel
					samples[c], // samples
					bufferSize, // max_num_samples
					&ready[c] // ready
				}
			);
		}

		for(size_t offset = 0; offset < file.getSize(); offset += bufferSize)
		{
			for(size_t c = 0; c < 13; ++c)
			{
				ready[c] = false;
			}

			size_t readSize = file.getSize() - offset;
			if(readSize > bufferSize)
			{
				readSize = bufferSize;
			}
			else
			{
				printf("Last read: %d samples\n", readSize);
			}

			file.readChunk(channels, offset, readSize);

			for(size_t c = 0; c < 13; ++c)
			{
				CPPUNIT_ASSERT_EQUAL(true, ready[c]?true:false);
			}

			sample_t diff[13] = {0.0};
			for(int c = 0; c < 13; ++c)
			{
				for(int i = 0; i < readSize; ++i)
				{
					diff[c] += abs(refFile[c]->data[i + offset] - samples[c][i]);
				}
			}

			for(int c = 0; c < 13; ++c)
			{
				CPPUNIT_ASSERT_EQUAL((sample_t)0.0, diff[c]);
			}
		}

		for(size_t c = 0; c < 13; ++c)
		{
			delete refFile[c];
		}
	}

	void readTest()
	{
		// Exhaustive test for 1...64
		for(size_t bufferSize = 1; bufferSize < 64; ++bufferSize)
		{
			readTestHelper(bufferSize);
		}

		// Binary test for 64 .. 4096
		for(size_t bufferSize = 64; bufferSize < 4096; bufferSize *= 2)
		{
			readTestHelper(bufferSize);
		}

		// And some sporadic tests for some "wierd" sizes.
		for(size_t bufferSize = 65; bufferSize < 4096; bufferSize *= 1.1)
		{
			readTestHelper(bufferSize);
		}
	}

};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(AudioCacheFileTest);

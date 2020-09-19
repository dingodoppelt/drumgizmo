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
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include <uunit.h>

#include <cstring>

#include <audiocachefile.h>
#include <audiofile.h>

#include "drumkit_creator.h"

class TestableAudioCacheFiles
	: public AudioCacheFiles
{
public:
	//CacheAudioFile& getAudioFile(const std::string& filename);
	//void release(const std::string& filename);
	int getRef(const std::string& filename)
	{
		auto it = audiofiles.find(filename);

		if(it == audiofiles.end())
		{
			return -1;
		}

		return (it->second).ref;
	}
};

class AudioCacheFileTest
	: public uUnit
{
public:
	AudioCacheFileTest()
	{
		uUNIT_TEST(AudioCacheFileTest::refTest);
		uUNIT_TEST(AudioCacheFileTest::readTest);
		uUNIT_TEST(AudioCacheFileTest::noFileTest);
	}

	DrumkitCreator drumkit_creator;

	void refTest()
	{
		// Create the audio file
		auto filename = drumkit_creator.createSingleChannelWav("single_channel.wav");

		// Conduct tests
		TestableAudioCacheFiles audiofiles;
		uUNIT_ASSERT_EQUAL(-1, audiofiles.getRef(filename));

		audiofiles.getFile(filename);
		uUNIT_ASSERT_EQUAL(1, audiofiles.getRef(filename));

		audiofiles.getFile(filename);
		uUNIT_ASSERT_EQUAL(2, audiofiles.getRef(filename));

		audiofiles.releaseFile(filename);
		uUNIT_ASSERT_EQUAL(1, audiofiles.getRef(filename));

		audiofiles.releaseFile(filename);
		uUNIT_ASSERT_EQUAL(-1, audiofiles.getRef(filename));
	}

	void readTestHelper(size_t buffer_size)
	{
		printf("Test buffer size: %d samples\n", (int)buffer_size);

		// Create the audio file
		auto filename = drumkit_creator.createMultiChannelWav("multi_channel.wav");

		// Conduct tests
		AudioFile* ref_file[13];
		for(size_t c = 0; c < 13; ++c)
		{
			ref_file[c] = new AudioFile(filename, c);
			ref_file[c]->load(nullptr);
		}

		std::vector<sample_t> read_buffer;

		AudioCacheFile file(filename, read_buffer);
		uUNIT_ASSERT_EQUAL(filename, file.getFilename());
		uUNIT_ASSERT_EQUAL(13, (int)file.getChannelCount()); // Sanity check

		CacheChannels channels;

		sample_t samples[13][buffer_size];
		volatile bool ready[13];
		for(size_t c = 0; c < 13; ++c)
		{
			for(size_t i = 0; i < buffer_size; ++i)
			{
				samples[c][i] = 42;
			}

			channels.push_back(
				{
					c, // channel
					samples[c], // samples
					buffer_size, // max_num_samples
					&ready[c] // ready
				}
			);
		}

		for(size_t offset = 0; offset < file.getSize(); offset += buffer_size)
		{
			for(size_t c = 0; c < 13; ++c)
			{
				ready[c] = false;
			}

			size_t read_size = file.getSize() - offset;
			if(read_size > buffer_size)
			{
				read_size = buffer_size;
			}
			else
			{
				printf("Last read: %d samples\n", (int)read_size);
			}

			file.readChunk(channels, offset, read_size);

			for(size_t c = 0; c < 13; ++c)
			{
				uUNIT_ASSERT_EQUAL(true, ready[c]?true:false);
			}

			sample_t diff[13] = {0.0};
			for(size_t c = 0; c < 13; ++c)
			{
				for(size_t i = 0; i < read_size; ++i)
				{
					diff[c] += abs((long)(ref_file[c]->data[i + offset] - samples[c][i]));
				}
			}

			for(int c = 0; c < 13; ++c)
			{
				uUNIT_ASSERT_EQUAL((sample_t)0.0, diff[c]);
			}
		}

		for(size_t c = 0; c < 13; ++c)
		{
			delete ref_file[c];
		}
	}

	void readTest()
	{
		// Exhaustive test for 1...64
		for(size_t buffer_size = 1; buffer_size < 64; ++buffer_size)
		{
			readTestHelper(buffer_size);
		}

		// Binary test for 64 .. 4096
		for(size_t buffer_size = 64; buffer_size < 4096; buffer_size *= 2)
		{
			readTestHelper(buffer_size);
		}

		// And some sporadic tests for some "wierd" sizes.
		for(size_t buffer_size = 65; buffer_size < 4096; buffer_size *= 1.1)
		{
			readTestHelper(buffer_size);
		}
	}

	void noFileTest()
	{
		size_t buffer_size = 64;
		std::string filename = "kits/no-such-file.wav";

		std::vector<sample_t> read_buffer;

		AudioCacheFile file(filename, read_buffer);
		uUNIT_ASSERT_EQUAL(filename, file.getFilename());
		uUNIT_ASSERT_EQUAL(0u, (unsigned int)file.getSize());
		uUNIT_ASSERT_EQUAL(0u, (unsigned int)file.getChannelCount());

		CacheChannels channels;

		sample_t samples[13][buffer_size];
		volatile bool ready[13];
		for(size_t c = 0; c < 13; ++c)
		{
			for(size_t i = 0; i < buffer_size; ++i)
			{
				samples[c][i] = 42.0f;
			}

			channels.push_back(
				{
					c, // channel
					samples[c], // samples
					buffer_size, // max_num_samples
					&ready[c] // ready
				}
			);
		}

		for(size_t c = 0; c < 13; ++c)
		{
			ready[c] = false;
		}

		file.readChunk(channels, 0, buffer_size);

		for(size_t c = 0; c < 13; ++c)
		{
			uUNIT_ASSERT_EQUAL(false, ready[c]?true:false);
		}

		for(size_t c = 0; c < 13; ++c)
		{
			for(size_t i = 0; i < buffer_size; ++i)
			{
				uUNIT_ASSERT_EQUAL(42.0f, samples[c][i]);
			}
		}
	}
};

// Registers the fixture into the 'registry'
static AudioCacheFileTest test;

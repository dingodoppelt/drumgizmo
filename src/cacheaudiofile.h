/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            cacheaudiofile.h
 *
 *  Thu Dec 24 12:17:58 CET 2015
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
#pragma once

#include <string>
#include <list>
#include <map>

#include <sndfile.h>

class CacheChannels;

class CacheAudioFile {
	friend class CacheAudioFiles;
public:
	CacheAudioFile(const std::string& filename);
	~CacheAudioFile();

	size_t getSize() const;
	const std::string& getFilename() const;

	void readChunk(const CacheChannels& channels,
	               size_t pos, size_t num_samples);

private:
	int ref{0};
	SNDFILE* fh{nullptr};
	SF_INFO sf_info;
	std::string filename;
};

class CacheAudioFiles {
public:
	CacheAudioFile& getAudioFile(const std::string filename);
	void release(const std::string filename);

private:
	std::map<std::string, CacheAudioFile*> audiofiles;
};

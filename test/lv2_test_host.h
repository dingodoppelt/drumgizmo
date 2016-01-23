/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            lv2_test_host.h
 *
 *  Wed Feb 11 23:11:20 CET 2015
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

#include <lilv/lilv.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>

class LV2TestHost {
public:
	class Sequence {
	public:
		Sequence(void *buffer, size_t buffer_size);
		void clear();
		void addMidiNote(uint64_t pos, uint8_t key, int8_t velocity);
		void *data();

	private:
		void *buffer;
		size_t buffer_size;
		LV2_Atom_Sequence *seq;
	};

	LV2TestHost(const char *lv2_path);
	~LV2TestHost();

	int open(const char *plugin_uri);
	int close();

	int verify();

	//void getMetadata();
	//int getPorts();

	int createInstance(size_t samplerate);
	int destroyInstance();

	int connectPort(int port, void *portdata);

	int activate();
	int deactivate();

	int loadConfig(const char *config, size_t size);
	int run(int num_samples);

private:
	LilvWorld* world;
	const LilvPlugins* plugins;
	LilvNode* uri;
	const LilvPlugin* plugin;

	LilvInstance* instance;
};

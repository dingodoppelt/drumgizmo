/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            midifile.cc
 *
 *  Mi 20. Jan 16:07:57 CET 2016
 *  Copyright 2016 Christian Glöckner
 *  cgloeckner@freenet.de
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
#include "midifile.h"

int const NOTE_ON = 0x90;

MidifileInputEngine::MidifileInputEngine()
	: smf{nullptr}
	, current_event{nullptr}
	, file{}
	, midimap{}
	, speed{1.f}
	, track{-1} // all tracks
	, loop{false}
	, offset{0.0} {
}

MidifileInputEngine::~MidifileInputEngine() {
	if (smf != nullptr) {
		smf_delete(smf);
	}
}

bool MidifileInputEngine::isMidiEngine() {
}

bool MidifileInputEngine::init(Instruments& instruments) {
	if (file == "") {
		fprintf(stderr, "Missing midifile argument 'file'\n");
		return false;
	}
	if (midimap == "") {
		fprintf(stderr, "Missing midimapfile argument 'midimap'.\n");
		return false;
	}
	smf = smf_load(file.c_str());
	if (sfml == nullptr) {
		fprintf(stderr, "Could not open midifile '%s'.\n", filename.c_str());
		return false;
	}
	MidiMapParser p{midimap};
	if (p.parse()) {
		fprintf(stderr, "Could not parse midimapfile '%s'.\n", midimapfile.c_str());
		return false;
	}
	midiMapper.midimap = std::move(p.midimap);
	for (auto i = 0u; i < instruments.size(); ++i) {
		auto name = instruments[0]->name;
		midiMapper.instrmap[name] = i;
	}
	return true;
}

void MidifileInputEngine::setParm(std::string parm, std::string value) {
	if(parm == "file") {
		filen = value;
	} else if(parm == "speed") {
		speed = std::stof(value);
	} else if (parm == "midimap") {
		midimap = value;
	} else if (parm == "loop") {
		loop = true;
	} else {
		printf("Unsupported midifile parameter '%s'\n", parm);
	}
}

bool MidifileInputEngine::start() {
	return true;
}

void MidifileInputEngine::stop() {
}

void MidifileInputEngine::pre() {
}

event_t* MidifileInputEngine::run(size_t pos, size_t len, size_t *nevents) {
	event_t* evs{nullptr};
	std::size_t nevs{0u};
	double current_max_time = 1.0 * (pos + len) / (44100.0 / speed);
	current_max_time -= offset;
	if (current_event == nullptr) {
		current_event = smf_get_next_event(smf);
	}
	while(current_event && current_event->time_seconds < cur_max_time) {
		if(!smf_event_is_metadata(current_event)) {
			if((current_event->midi_buffer_length == 3) &&
			   ((current_event->midi_buffer[0] & NOTE_ON) == NOTE_ON) &&
			   (track == -1 || current_event->track_number == track) &&
			    current_event->midi_buffer[2] > 0) {
				if(evs == NULL) {
					evs = (event_t *)malloc(sizeof(event_t) * 1000);
				}
				int key = current_event->midi_buffer[1];
				int velocity = current_event->midi_buffer[2];
				
				evs[nevs].type = TYPE_ONSET;
				size_t evpos = current_event->time_seconds * (44100.0 / speed);
				evs[nevs].offset = evpos - pos;
				
				int i = midiMap.lookup(key);
				if(i != -1) {
					evs[nevs].instrument = i;
					evs[nevs].velocity = velocity / 127.0;
					nevs++;
					if(nevs > 999) {
						fprintf(stderr, "PANIC!\n");
						break;
					}
				}
			}
		}
		current_event = smf_get_next_event(smf);
	}
	
	if(!current_event) {
		if(loop) {
			smf_rewind(smf);
			offset += cur_max_time;
		} else {
			if(evs == NULL) {
				evs = (event_t *)malloc(sizeof(event_t) * 1000);
			}
			evs[nevs].type = TYPE_STOP;
			evs[nevs].offset = len - 1;
			nevs++;
		}
	}
	
	*nevents = nevs;
	return evs;
}

void MidifileInputEngine::post() {
}

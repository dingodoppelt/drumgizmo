/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            inputprocessor.cc
 *
 *  Sat Apr 23 20:39:30 CEST 2016
 *  Copyright 2016 André Nusser
 *  andre.nusser@googlemail.com
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
#include "inputprocessor.h"

#include <list>

#include "hugin.hpp"

#include "instrument.h"

InputProcessor::InputProcessor(DrumKit& kit, std::list<Event*>* activeevents)
	: kit(kit)
	, activeevents(activeevents)
	, is_stopping(false)
{

}

bool InputProcessor::process(const std::vector<event_t>& events, size_t pos, double resample_ratio)
{
	for(const auto& event: events)
	{
		if(event.type == TYPE_ONSET)
		{
			if(!processOnset(event, pos, resample_ratio))
			{
				continue;
			}
		}

		if(!processStop(event))
		{
			return false;
		}
	}

	return true;
}

std::size_t InputProcessor::getLatency() const
{
	return 0;
}

bool InputProcessor::processOnset(const event_t& event, size_t pos, double resample_ratio)
{
	if(!kit.isValid()) {
		return false;
	}

	size_t ev_instr = event.instrument;
	Instrument* instr = nullptr;

	if(ev_instr < kit.instruments.size())
	{
		instr = kit.instruments[ev_instr].get();
	}

	if(instr == nullptr || !instr->isValid())
	{
		ERR(inputprocessor, "Missing Instrument %d.\n", (int)ev_instr);
		return false;
	}

	if(instr->getGroup() != "")
	{
		// Add event to ramp down all existing events with the same groupname.
		for(Channel& ch: kit.channels)
		{
			for(Event* event: activeevents[ch.num])
			{
				if(event->getType() == Event::sample)
				{
					auto& event_sample = *static_cast<EventSample*>(event);
					if(event_sample.group == instr->getGroup() &&
					   event_sample.instrument != instr)
					{
						event_sample.rampdown = 3000; // Ramp down 3000 samples
						// TODO: This must be configurable at some point...
						// ... perhaps even by instrument (ie. in the xml file)
						event_sample.ramp_start = event_sample.rampdown;
					}
				}
			}
		}
	}

	Sample* sample = instr->sample(event.velocity, event.offset + pos);

	if(sample == nullptr)
	{
		ERR(inputprocessor, "Missing Sample.\n");
		return false;
	}

	for(Channel& ch: kit.channels)
	{
		AudioFile* af = sample->getAudioFile(&ch);
		if(af == nullptr || !af->isValid())
		{
			//DEBUG(inputprocessor, "Missing AudioFile.\n");
		}
		else
		{
			//DEBUG(inputprocessor, "Adding event %d.\n", event.offset);
			Event* evt = new EventSample(ch.num, 1.0, af, instr->getGroup(), instr);
			evt->offset = (event.offset + pos) * resample_ratio;
			activeevents[ch.num].push_back(evt);
		}
	}

	return true;
}

bool InputProcessor::processStop(const event_t& event)
{
	if(event.type == TYPE_STOP)
	{
		is_stopping = true;
	}

	if(is_stopping)
	{
		// Count the number of active events.
		int num_active_events = 0;
		for(auto& ch: kit.channels)
		{
			num_active_events += activeevents[ch.num].size();
		}

		if(num_active_events == 0)
		{
			// No more active events - now we can stop the engine.
			return false;
		}
	}

	return true;
}

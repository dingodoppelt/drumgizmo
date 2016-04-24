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
			if(!process_onset(event, pos, resample_ratio))
			{
				continue;
			}
		}

		if(!process_stop(event))
		{
			return false;
		}
	}

	return true;
}

bool InputProcessor::process_onset(const event_t& event, size_t pos, double resample_ratio)
{
	Instrument* i = nullptr;
	int d = event.instrument;

	// TODO can this be removed?
	if(!kit.isValid())
	{
		return false;
	}

	if(d < (int)kit.instruments.size())
	{
		i = kit.instruments[d];
	}

	if(i == nullptr || !i->isValid())
	{
		ERR(inputprocessor, "Missing Instrument %d.\n", (int)event.instrument);
		return false;
	}

	if(i->getGroup() != "")
	{
		// Add event to ramp down all existing events with the same groupname.
		Channels::iterator j = kit.channels.begin();
		while(j != kit.channels.end())
		{
			Channel &ch = *j;
			std::list< Event* >::iterator evs = activeevents[ch.num].begin();
			while(evs != activeevents[ch.num].end())
			{
				Event *ev = *evs;
				if(ev->getType() == Event::sample)
				{
					EventSample *sev = (EventSample*)ev;
					if(sev->group == i->getGroup() && sev->instrument != i)
					{
						sev->rampdown = 3000; // Ramp down 3000 samples
						// TODO: This must be configurable at some point...
						// ... perhaps even by instrument (ie. in the xml file)
						sev->ramp_start = sev->rampdown;
					}
				}
				++evs;
			}
			++j;
		}
	}

	Sample *s = i->sample(event.velocity, event.offset + pos);

	if(s == nullptr)
	{
		ERR(inputprocessor, "Missing Sample.\n");
		return false;
	}

	Channels::iterator j = kit.channels.begin();
	while(j != kit.channels.end())
	{
		Channel &ch = *j;
		AudioFile *af = s->getAudioFile(&ch);
		if(af)
		{
			// LAZYLOAD:
			// DEBUG(inputprocessor, "Requesting preparing of audio file\n");
			// loader.prepare(af);
		}
		if(af == nullptr || !af->isValid())
		{
			//DEBUG(inputprocessor, "Missing AudioFile.\n");
		}
		else
		{
			//DEBUG(inputprocessor, "Adding event %d.\n", event.offset);
			Event *evt = new EventSample(ch.num, 1.0, af, i->getGroup(), i);
			evt->offset = (event.offset + pos) * resample_ratio;
			activeevents[ch.num].push_back(evt);
		}
		++j;
	}

	return true;
}

bool InputProcessor::process_stop(const event_t& event)
{
	if(event.type == TYPE_STOP)
	{
		is_stopping = true;
	}

	if(is_stopping)
	{
		// Count the number of active events.
		int num_active_events = 0;
		Channels::iterator j = kit.channels.begin();
		while(j != kit.channels.end())
		{
			Channel &ch = *j;
			num_active_events += activeevents[ch.num].size();
			++j;
		}

		if(num_active_events == 0)
		{
			// No more active events - now we can stop the engine.
			return false;
		}
	}

	return true;
}

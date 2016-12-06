/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            instrumentparser.h
 *
 *  Wed Mar  9 13:22:24 CET 2011
 *  Copyright 2011 Bent Bisballe Nyeng
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
#pragma once

#include "saxparser.h"
#include "instrument.h"

#include <memory>
#include <vector>

class InstrumentParser
	: public SAXParser
{
public:
	InstrumentParser(Instrument &instrument);
	virtual ~InstrumentParser() = default;

	virtual int parseFile(const std::string& filename) override;

	std::vector<InstrumentChannel*> channellist;

protected:
	virtual void startTag(const std::string& name, const attr_t& attr) override;
	virtual void endTag(const std::string& name) override;

private:
	Instrument& instrument;
	Sample* sample{nullptr};

	std::string path;

	level_t lower{0};
	level_t upper{0};
};

/* -*- Mode: c++ -*- */
/***************************************************************************
 *            domloader.h
 *
 *  Sun Jun 10 17:39:01 CEST 2018
 *  Copyright 2018 Bent Bisballe Nyeng
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

#include <vector>
#include <string>

struct DrumkitDOM;
struct InstrumentDOM;
class DrumKit;
struct Settings;
class Random;
class InstrumentChannel;
class Instrument;

class DOMLoader
{
public:
	DOMLoader(Settings& settings, Random& random);

	bool loadDom(const DrumkitDOM& dom,
	             const std::vector<InstrumentDOM>& instrumentdoms,
	             DrumKit& drumkit);

private:
	static InstrumentChannel* addOrGetChannel(Instrument& instrument,
	                                          const std::string& name);

	Settings& settings;
	Random& random;
};

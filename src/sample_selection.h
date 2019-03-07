/* -*- Mode: c++ -*- */
/***************************************************************************
 *            sample_selection.h
 *
 *  Mon Mar  4 23:58:12 CET 2019
 *  Copyright 2019 André Nusser
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
#pragma once

#include <vector>

#include "sample.h"

class PowerList;
class Random;
struct Settings;

enum class SelectionAlg
{
	Old,
	Objective,
};

class SampleSelection
{
private:
	Settings& settings;
	Random& rand;
	const PowerList& powerlist;

	Sample* lastsample;
	std::vector<std::size_t> last;

	SelectionAlg alg;
	const Sample* getOld(level_t level, std::size_t pos);
	const Sample* getObjective(level_t level, std::size_t pos);

public:
	SampleSelection(Settings& settings, Random& rand, const PowerList& powerlist);

	void setSelectionAlg(SelectionAlg alg);
	void finalise();
	const Sample* get(level_t level, std::size_t pos);
};

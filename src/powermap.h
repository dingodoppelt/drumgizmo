/* -*- Mode: c++ -*- */
/***************************************************************************
 *            powermap.h
 *
 *  Fri Apr 17 23:06:12 CEST 2020
 *  Copyright 2020 André Nusser
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

#include <array>
#include <vector>

class Powermap
{
public:
	using Power = float;
	using Powers = std::vector<Power>;
	struct PowerPair
	{
		Power in;
		Power out;

		bool operator!=(const PowerPair& other)
		{
			return in != other.in || out != other.out;
		}
	};

	Powermap();

	Power map(Power in);
	void reset();

	void setFixed0(PowerPair new_value);
	void setFixed1(PowerPair new_value);
	void setFixed2(PowerPair new_value);
	void setShelf(bool enable);

	PowerPair getFixed0() const;
	PowerPair getFixed1() const;
	PowerPair getFixed2() const;

private:
	// input parameters (state of this class)
	std::array<PowerPair, 3> fixed;
	bool shelf;

	// spline parameters (deterministically computed from the input parameters)
	bool spline_needs_update;
	std::array<float, 5> m;
	const Power eps = 1e-4;

	void updateSpline();
	std::vector<float> calcSlopes(const Powers& X, const Powers& P);

	Power clamp(Power in, Power min, Power max) const;
};

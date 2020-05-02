/* -*- Mode: c++ -*- */
/***************************************************************************
 *            powermap.cc
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
#include "powermap.h"

#include <cassert>
#include <cmath>

namespace
{

using Power = Powermap::Power;
using PowerPair = Powermap::PowerPair;

Power h00(Power x)
{
	return (1 + 2 * x) * pow(1 - x, 2);
}

Power h10(Power x)
{
	return x * pow(1 - x, 2);
}

Power h01(Power x)
{
	return x * x * (3 - 2 * x);
}

Power h11(Power x)
{
	return x * x * (x - 1);
}

Power computeValue(const Power x, const PowerPair& P0, const PowerPair& P1,
                   const Power m0, const Power m1)
{
	const auto x0 = P0.in;
	const auto x1 = P1.in;
	const auto y0 = P0.out;
	const auto y1 = P1.out;
	const auto dx = x1 - x0;
	const auto x_prime = (x - x0)/dx;

	return
		h00(x_prime) * y0 +
		h10(x_prime) * dx * m0 +
		h01(x_prime) * y1 +
		h11(x_prime) * dx * m1;
}

} // end anonymous namespace

Powermap::Powermap()
{
	reset();
}

Power Powermap::map(Power in)
{
	assert(in >= 0. && in <= 1.);

	if (spline_needs_update)
	{
		updateSpline();
	}

	Power out;
	if (in < fixed[0].in)
	{
		out = shelf ? fixed[0].out
		            : computeValue(in, {0.,0.}, fixed[0], m[0], m[1]);
	}
	else if (in < fixed[1].in)
	{
		out = computeValue(in, fixed[0], fixed[1], m[1], m[2]);
	}
	else if (in < fixed[2].in)
	{
		out = computeValue(in, fixed[1], fixed[2], m[2], m[3]);
	}
	else
	{
		// in >= fixed[2].in
		out = shelf ? fixed[2].out
		            : computeValue(in, fixed[2], {1.,1.}, m[3], m[4]);
	}

	assert(out >= 0. && out <= 1.);
	return out;
}

void Powermap::reset()
{
	setFixed0({eps, eps});
	setFixed1({.5, .5});
	setFixed2({1 - eps, 1 - eps});
	// FIXME: better false?
	shelf = true;

	updateSpline();
}

void Powermap::setFixed0(PowerPair new_value)
{
	if (fixed[0] != new_value)
	{
		spline_needs_update = true;
		fixed[0].in = clamp(new_value.in, eps, fixed[1].in - eps);
		fixed[0].out = clamp(new_value.out, eps, fixed[1].out - eps);
	}
}

void Powermap::setFixed1(PowerPair new_value)
{
	if (fixed[1] != new_value)
	{
		spline_needs_update = true;
		fixed[1].in = clamp(new_value.in, fixed[0].in + eps, fixed[2].in - eps);
		fixed[1].out = clamp(new_value.out, fixed[0].out + eps, fixed[2].out - eps);
	}
}

void Powermap::setFixed2(PowerPair new_value)
{
	if (fixed[2] != new_value)
	{
		spline_needs_update = true;
		fixed[2].in = clamp(new_value.in, fixed[1].in + eps, 1 - eps);
		fixed[2].out = clamp(new_value.out, fixed[1].out + eps, 1 - eps);
	}
}

void Powermap::setShelf(bool enable)
{
	if (shelf != enable)
	{
		spline_needs_update = true;
		this->shelf = enable;
	}
}

PowerPair Powermap::getFixed0() const
{
	return fixed[0];
}

PowerPair Powermap::getFixed1() const
{
	return fixed[1];
}

PowerPair Powermap::getFixed2() const
{
	return fixed[2];
}

// This mostly followes the wikipedia article for monotone cubic splines:
// https://en.wikipedia.org/wiki/Monotone_cubic_interpolation
void Powermap::updateSpline()
{
	assert(0. <= fixed[0].in && fixed[0].in < fixed[1].in &&
	       fixed[1].in < fixed[2].in && fixed[2].in <= 1.);
	assert(0. <= fixed[0].out && fixed[0].out <= fixed[1].out &&
	       fixed[1].out <= fixed[2].out && fixed[2].out <= 1.);

	Powers X = shelf ? Powers{fixed[0].in, fixed[1].in, fixed[2].in}
	                 : Powers{0., fixed[0].in, fixed[1].in, fixed[2].in, 1.};
	Powers Y = shelf ? Powers{fixed[0].out, fixed[1].out, fixed[2].out}
	                 : Powers{0., fixed[0].out, fixed[1].out, fixed[2].out, 1.};

	auto slopes = calcSlopes(X, Y);

	if (shelf)
	{
		assert(slopes.size() == 3);
		this->m[1] = slopes[0];
		this->m[2] = slopes[1];
		this->m[3] = slopes[2];
	}
	else
	{
		assert(slopes.size() == 5);
		for (std::size_t i = 0; i < m.size(); ++i)
		{
			this->m[i] = slopes[i];
		}
	}

	spline_needs_update = false;
}

// This follows the monotone cubic spline algorithm of Steffen, from:
// "A Simple Method for Monotonic Interpolation in One Dimension"
std::vector<float> Powermap::calcSlopes(const Powers& X, const Powers& Y)
{
	Powers m(X.size());

	Powers d(X.size() - 1);
	Powers h(X.size() - 1);
	for (std::size_t i = 0; i < d.size(); ++i)
	{
		h[i] = X[i + 1] - X[i];
		d[i] = (Y[i + 1] - Y[i]) / h[i];
	}

	m.front() = d.front();
	for (std::size_t i = 1; i < m.size() - 1; ++i)
	{
		m[i] = (d[i - 1] + d[i]) / 2.;
	}
	m.back() = d.back();

	for (std::size_t i = 1; i < m.size() - 1; ++i)
	{
		const auto min_d = 2*std::min(d[i - 1], d[i]);
		m[i] =
			std::min<float>(min_d,
			                (h[i] * d[i - 1] + h[i - 1] * d[i]) / (h[i - 1] + h[i]));
	}

	return m;
}

Power Powermap::clamp(Power in, Power min, Power max) const
{
	return std::max(min, std::min(in, max));
}

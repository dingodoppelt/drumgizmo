/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            chresampler.h
 *
 *  Tue Sep 23 20:42:14 CEST 2014
 *  Copyright 2014 Bent Bisballe Nyeng
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

#include <memory>
#include <config.h>
#include <array>

//! Channel resampler class using either zita-resampler or secret rabbit code
//! (really!!) depending on the value of the WITH_RESAMPLER macro.
//! If WITH_RESAMPLER is unset the resampler is disabled entirely.
//! If WITH_RESAMPLER=="zita" zita-resampler will be used.
//! If WITH_RESAMPLER=="src" Secret Rabbit Code will be used.
class CHResampler
{
public:
	CHResampler();
	~CHResampler();

	void setup(double input_fs, double output_fs);

	void setInputSamples(float* samples, std::size_t count);
	void setOutputSamples(float* samples, std::size_t count);

	void process();

	std::size_t getLatency() const;

	std::size_t getInputSampleCount() const;
	std::size_t getOutputSampleCount() const;

	double getRatio() const;

#ifdef WITH_RESAMPLER
private:
	class Prv;
	std::unique_ptr<Prv> prv;

	double input_fs{44100.0};
	double output_fs{44100.0};
#endif /*WITH_RESAMPLER*/
};

//! Container class for the resampler array.
class Resamplers
{
public:
	void setup(double input_fs, double output_fs)
	{
		for(auto& resampler : resamplers)
		{
			resampler.setup(input_fs, output_fs);
		}
	}

	bool isActive() const
	{
		return getRatio() != 1.0;
	}

	double getRatio() const
	{
		return resamplers[0].getRatio();
	}


	CHResampler& operator[](std::size_t idx)
	{
		return resamplers[idx];
	}

	std::size_t getOutputSampleCount() const
	{
		return resamplers[0].getOutputSampleCount();
	}

	std::size_t getLatency() const
	{
		return resamplers[0].getLatency();
	}

	std::array<CHResampler, 64> resamplers;
};

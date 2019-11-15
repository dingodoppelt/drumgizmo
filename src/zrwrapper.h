/* -*- Mode: c++ -*- */
/***************************************************************************
 *            zrwrapper.h
 *
 *  Fri Nov 15 15:06:51 CET 2019
 *  Copyright 2019 Bent Bisballe Nyeng
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

class ZRWrapper
{
public:
	ZRWrapper();
	~ZRWrapper();

	int setup(unsigned int fs_inp,
	          unsigned int fs_out,
	          unsigned int nchan,
	          unsigned int hlen);

	int setup(unsigned int fs_inp,
	          unsigned int fs_out,
	          unsigned int nchan,
	          unsigned int hlen,
	          double frel);

	void clear();
	int reset();
	int nchan() const;
	int filtlen() const;
	int inpsize() const;
	double inpdist() const;
	int process();

	unsigned int get_inp_count();
	void set_inp_count(unsigned int inp_count);

	unsigned int get_out_count();
	void set_out_count(unsigned int out_count);

	float *get_inp_data();
	void set_inp_data(float *inp_data);

	float *get_out_data();
	void set_out_data(float *out_data);

	void *get_inp_list();
	void set_inp_list(void *inp_list);

	void *get_out_list();
	void set_out_list(void *out_list);

private:
	std::unique_ptr<class Resampler> resampler;
};

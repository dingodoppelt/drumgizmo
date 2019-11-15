/* -*- Mode: c++ -*- */
/***************************************************************************
 *            zrwrapper.cc
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
#include "zrwrapper.h"

#include <zita-resampler/resampler.h>

#include "cpp11fix.h"

ZRWrapper::ZRWrapper()
	: resampler(std::make_unique<Resampler>())
{
}

ZRWrapper::~ZRWrapper() = default;

int ZRWrapper::setup(unsigned int fs_inp,
                     unsigned int fs_out,
                     unsigned int nchan,
                     unsigned int hlen)
{
	return resampler->setup(fs_inp, fs_out, nchan, hlen);
}

int ZRWrapper::setup(unsigned int fs_inp,
                     unsigned int fs_out,
                     unsigned int nchan,
                     unsigned int hlen,
                     double frel)
{
	return resampler->setup(fs_inp, fs_out, nchan, hlen, frel);
}

void ZRWrapper::clear()
{
	resampler->clear();
}

int ZRWrapper::reset()
{
	return resampler->reset();
}

int ZRWrapper::nchan() const
{
	return resampler->nchan();
}

int ZRWrapper::filtlen() const
{
	return resampler->filtlen();
}

int ZRWrapper::inpsize() const
{
	return resampler->inpsize();
}

double ZRWrapper::inpdist() const
{
	return resampler->inpdist();
}

int ZRWrapper::process()
{
	return resampler->process();
}

unsigned int ZRWrapper::get_inp_count()
{
	return resampler->inp_count;
}

void ZRWrapper::set_inp_count(unsigned int inp_count)
{
	resampler->inp_count = inp_count;
}

unsigned int ZRWrapper::get_out_count()
{
	return resampler->out_count;
}

void ZRWrapper::set_out_count(unsigned int out_count)
{
	resampler->out_count = out_count;
}

float *ZRWrapper::get_inp_data()
{
	return resampler->inp_data;
}

void ZRWrapper::set_inp_data(float *inp_data)
{
	resampler->inp_data = inp_data;
}

float *ZRWrapper::get_out_data()
{
	return resampler->out_data;
}

void ZRWrapper::set_out_data(float *out_data)
{
	resampler->out_data = out_data;
}

void *ZRWrapper::get_inp_list()
{
	return resampler->inp_list;
}

void ZRWrapper::set_inp_list(void *inp_list)
{
	resampler->inp_list = inp_list;
}

void *ZRWrapper::get_out_list()
{
	return resampler->out_list;
}

void ZRWrapper::set_out_list(void *out_list)
{
	resampler->out_list = out_list;
}

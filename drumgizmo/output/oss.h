/* -*- Mode: c++ -*- */
/***************************************************************************
 *            oss.h
 *
 *  Tue Apr 11 19:42:45 CET 2017
 *  Copyright 2017 Goran Mekić
 *  meka@tilda.center
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
#include "audiooutputengine.h"

class OSSOutputEngine
	: public AudioOutputEngine
{
public:
	OSSOutputEngine();
	~OSSOutputEngine() {};

	// based on AudioOutputEngine
	bool init(const Channels& chan) override;
	void setParm(const std::string& parm, const std::string& value) override;
	bool start() override { return true; };
	void stop() override {};
	void pre(size_t nsamples) override;
	void run(int ch, sample_t* samples, size_t nsamples) override;
	void post(size_t nsamples) override;
	bool isFreewheeling() const override { return false; };
	std::size_t getBufferSize() const override;
	std::size_t getSamplerate() const override;

private:
	std::string dev;
	int fd;
	std::size_t num_channels;
	unsigned int srate;
	unsigned int format;
	std::size_t max_fragments;
	std::size_t fragment_size;
	std::size_t buffer_size;
	std::vector<std::int32_t> data;
};

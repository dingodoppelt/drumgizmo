/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            outputdummy.cc
 *
 *  Fr 22. Jan 08:15:04 CET 2016
 *  Copyright 2016 Christian Gl�ckner
 *  cgloeckner@freenet.de
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
#include "outputdummy.h"

DummyOutputEngine::DummyOutputEngine()
{
}

DummyOutputEngine::~DummyOutputEngine()
{
}

bool DummyOutputEngine::init(const Channels& data)
{
	return true;
}

void DummyOutputEngine::setParm(const std::string& parm, const std::string& value)
{
}

bool DummyOutputEngine::start()
{
	return true;
}

void DummyOutputEngine::stop()
{
}

void DummyOutputEngine::pre(size_t nsamples)
{
}

void DummyOutputEngine::run(int ch, sample_t* samples, size_t nsamples)
{
}

void DummyOutputEngine::post(size_t nsamples)
{
}

std::size_t DummyOutputEngine::getSamplerate() const
{
	return 44100;
}

bool DummyOutputEngine::isFreewheeling() const
{
	return true;
}

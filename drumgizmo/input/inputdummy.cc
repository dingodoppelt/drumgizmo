/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            inputdummy.cc
 *
 *  Fr 22. Jan 08:14:58 CET 2016
 *  Copyright 2016 Christian Glöckner
 *  cgloeckner@freenet.de
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "inputdummy.h"

DummyInputEngine::DummyInputEngine() {}

DummyInputEngine::~DummyInputEngine() {}

bool DummyInputEngine::init(Instruments &instruments) { return true; }

void DummyInputEngine::setParm(std::string parm, std::string value) {}

bool DummyInputEngine::start() { return true; }

void DummyInputEngine::stop() {}

void DummyInputEngine::pre() {}

event_t *DummyInputEngine::run(size_t pos, size_t len, size_t *nevents)
{
	nevents = 0u;
	return nullptr;
}

void DummyInputEngine::post() {}

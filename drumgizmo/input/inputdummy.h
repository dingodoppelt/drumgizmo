/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            inputdummy.h
 *
 *  Fr 22. Jan 08:14:58 CET 2016
 *  Copyright 2016 Christian Gl�ckner
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
#pragma once
#include <audioinputengine.h>

class DummyInputEngine
	: public AudioInputEngine
{
public:
	DummyInputEngine();
	~DummyInputEngine();

	// based on AudioInputEngine
	bool init(Instruments& instruments) override;
	void setParm(std::string parm, std::string value) override;
	bool start() override;
	void stop() override;
	void pre() override;
	event_t* run(size_t pos, size_t len, size_t* nevents) override;
	void post() override;
};
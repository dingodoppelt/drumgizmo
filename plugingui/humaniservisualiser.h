/* -*- Mode: c++ -*- */
/***************************************************************************
 *            humaniservisualiser.h
 *
 *  Fri Jun 15 19:09:18 CEST 2018
 *  Copyright 2018 Bent Bisballe Nyeng
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

#include "widget.h"

class SettingsNotifier;

class HumaniserVisualiser
	: public GUI::Widget
{
public:
	HumaniserVisualiser(GUI::Widget* parent,
	                    SettingsNotifier& settings_notifier);

	// From Widget:
	virtual void repaintEvent(GUI::RepaintEvent *repaintEvent) override;

	void latencyOffsetChanged(int offset);
	void velocityOffsetChanged(float offset);
	void latencyStddevChanged(float stddev);
	void latencyLaidbackChanged(int laidback);
	void velocityStddevChanged(float stddev);

private:
	int latency_offset;
	float velocity_offset;
	float latency_stddev;
	int laidback;
	float velocity_stddev;
	SettingsNotifier& settings_notifier;
};

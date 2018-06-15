/* -*- Mode: c++ -*- */
/***************************************************************************
 *            humaniservisualiser.cc
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
#include "humaniservisualiser.h"

#include "painter.h"

#include <notifier.h>
#include <settings.h>

#include <iostream>

HumaniserVisualiser::HumaniserVisualiser(GUI::Widget* parent,
                                         SettingsNotifier& settings_notifier)
	: GUI::Widget(parent)
	, settings_notifier(settings_notifier)
{
	CONNECT(this, settings_notifier.latency_current,
	        this, &HumaniserVisualiser::latencyOffsetChanged);
	CONNECT(this, settings_notifier.velocity_modifier_current,
	        this, &HumaniserVisualiser::velocityOffsetChanged);

	CONNECT(this, settings_notifier.latency_stddev,
	        this, &HumaniserVisualiser::latencyStddevChanged);
	CONNECT(this, settings_notifier.latency_laid_back,
	        this, &HumaniserVisualiser::latencyLaidbackChanged);
	CONNECT(this, settings_notifier.velocity_stddev,
	        this, &HumaniserVisualiser::velocityStddevChanged);
}

void HumaniserVisualiser::repaintEvent(GUI::RepaintEvent *repaintEvent)
{
	GUI::Painter p(*this);

	// Background
	p.setColour(GUI::Colour(0, 0, 1));
	p.drawFilledRectangle(0, 0, width(), height());

	int x = latency_offset / 2000.0 * width() / 2 + width() / 2;
	int y = velocity_offset * height();
	int w = latency_stddev / 10;
	int h = velocity_stddev * 10;

	// Stddev squares
	float v = w;
	while(v > 1)
	{
		float a = 1.0f - v / (float)w;
		a = a * a * a;
		p.setColour(GUI::Colour(1.0f, 0.0f, 1.0f, a));
		p.drawFilledRectangle(x - v / 2, 0,
		                      x + v / 2, height());
		v -= 1.0f;
	}

	v = h;
	while(v > 1)
	{
		float a = 1.0f - v / (float)h;
		a = a * a * a;
		p.setColour(GUI::Colour(1.0f, 0.0f, 1.0f, a));
		p.drawFilledRectangle(0, y - v / 2,
		                      width(), y + v / 2);
		v -= 1.0f;
	}

	// Lines
	p.setColour(GUI::Colour(0, 1, 1));
	p.drawLine(0, y, width(), y);
	p.drawLine(x, 0, x, height());
}

void HumaniserVisualiser::latencyOffsetChanged(int offset)
{
	latency_offset = offset;
	redraw();
}

void HumaniserVisualiser::velocityOffsetChanged(float offset)
{
	velocity_offset = -1 * offset + 1;
	redraw();
}

void HumaniserVisualiser::latencyStddevChanged(float stddev)
{
	latency_stddev = stddev;
	redraw();
}

void HumaniserVisualiser::latencyLaidbackChanged(int laidback)
{
	this->laidback = laidback;
	redraw();
}

void HumaniserVisualiser::velocityStddevChanged(float stddev)
{
	velocity_stddev = stddev;
	redraw();
}

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

#include <hugin.hpp>

HumaniserVisualiser::HumaniserVisualiser(GUI::Widget* parent,
                                         Settings& settings,
                                         SettingsNotifier& settings_notifier)
	: GUI::Widget(parent)
	, canvas(this, settings, settings_notifier)
{
	canvas.move(7, 7);
}

void HumaniserVisualiser::repaintEvent(GUI::RepaintEvent *repaintEvent)
{
	GUI::Painter p(*this);

	box.setSize(width(), height());
	p.drawImage(0, 0, box);
}

void HumaniserVisualiser::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);
	if(width < 14 || height < 14)
	{
		canvas.resize(1, 1);
		return;
	}
	canvas.resize(width - 14, height - 14);
}

HumaniserVisualiser::Canvas::Canvas(GUI::Widget* parent,
                                    Settings& settings,
                                    SettingsNotifier& settings_notifier)
	: GUI::Widget(parent)
	, settings_notifier(settings_notifier)
	, latency_max_samples(settings.latency_max_ms.load() *
	                      settings.samplerate.load() / 1000)
	, settings(settings)
{
	CONNECT(this, settings_notifier.enable_latency_modifier,
	        this, &HumaniserVisualiser::Canvas::latencyEnabledChanged);
	CONNECT(this, settings_notifier.enable_velocity_modifier,
	        this, &HumaniserVisualiser::Canvas::velocityEnabledChanged);

	CONNECT(this, settings_notifier.latency_current,
	        this, &HumaniserVisualiser::Canvas::latencyOffsetChanged);
	CONNECT(this, settings_notifier.velocity_modifier_current,
	        this, &HumaniserVisualiser::Canvas::velocityOffsetChanged);

	CONNECT(this, settings_notifier.latency_stddev,
	        this, &HumaniserVisualiser::Canvas::latencyStddevChanged);
	CONNECT(this, settings_notifier.latency_laid_back_ms,
	        this, &HumaniserVisualiser::Canvas::latencyLaidbackChanged);
	CONNECT(this, settings_notifier.velocity_stddev,
	        this, &HumaniserVisualiser::Canvas::velocityStddevChanged);
}

void HumaniserVisualiser::Canvas::repaintEvent(GUI::RepaintEvent *repaintEvent)
{
	if(width() < 1 || height() < 1)
	{
		return;
	}

	GUI::Painter p(*this);

	p.clear();

	const int spx = latency_max_samples * 2 / width(); // samples pr. pixel

	int x = latency_offset / spx + width() / 2;
	float v = (-1.0f * velocity_offset + 1.0f) * 0.8;
	int y = height() * 0.2 + v * height();
	y = std::max(0, y);
	int w = 50.f * latency_stddev / spx * 3 * 2; // stddev is ~ +/- 3 span
	int h = velocity_stddev * height() / 4;

	DEBUG(vis, "max: %d, spx: %d, x: %d, w: %d", latency_max_samples, spx, x, w);

	// Stddev squares
	if(latency_enabled)
	{
		p.drawImageStretched(x - w / 2, 0, stddev_h, w, height());
	}
	else
	{
		p.drawImageStretched(x - w / 2, 0, stddev_h_disabled, w, height());
	}

	if(velocity_enabled)
	{
		p.drawImageStretched(0, y - h / 2, stddev_v, width(), h);
	}
	else
	{
		p.drawImageStretched(0, y - h / 2, stddev_v_disabled, width(), h);
	}

	// Lines
	if(velocity_enabled)
	{
		p.setColour(GUI::Colour(0.0f, 1.0f, 1.0f));
	}
	else
	{
		p.setColour(GUI::Colour(0.4f, 0.4f, 0.4f));
	}
	p.drawLine(0, y, width(), y);

	if(latency_enabled)
	{
		p.setColour(GUI::Colour(0.0f, 1.0f, 1.0f));
	}
	else
	{
		p.setColour(GUI::Colour(0.4f, 0.4f, 0.4f));
	}
	p.drawLine(x, 0, x, height());

	// Zero-lines
	p.setColour(GUI::Colour(0.0f, 1.0f, 0.0f, 0.9f));
	p.drawLine(0, height() * 0.2f, width(), height() * 0.2f);
	p.drawLine(width() / 2, 0, width() / 2, height());
}

void HumaniserVisualiser::Canvas::latencyEnabledChanged(bool enabled)
{
	latency_enabled = enabled;
	redraw();
}

void HumaniserVisualiser::Canvas::velocityEnabledChanged(bool enabled)
{
	velocity_enabled = enabled;
	redraw();
}

void HumaniserVisualiser::Canvas::latencyOffsetChanged(int offset)
{
	latency_offset = offset;
	redraw();
}

void HumaniserVisualiser::Canvas::velocityOffsetChanged(float offset)
{
	velocity_offset = offset;
	redraw();
}

void HumaniserVisualiser::Canvas::latencyStddevChanged(float stddev)
{
	latency_stddev = stddev;
	redraw();
}

void HumaniserVisualiser::Canvas::latencyLaidbackChanged(float laidback_ms)
{
	this->laidback = laidback_ms * settings.samplerate.load() / 1000;
	redraw();
}

void HumaniserVisualiser::Canvas::velocityStddevChanged(float stddev)
{
	velocity_stddev = stddev;
	redraw();
}

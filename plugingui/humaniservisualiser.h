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

#include <dggui/widget.h>
#include <dggui/texturedbox.h>
#include <dggui/texture.h>

struct Settings;
class SettingsNotifier;

class HumaniserVisualiser
	: public GUI::Widget
{
public:
	HumaniserVisualiser(GUI::Widget* parent,
	                    Settings& settings,
	                    SettingsNotifier& settings_notifier);

	// From Widget:
	virtual void repaintEvent(GUI::RepaintEvent *repaintEvent) override;
	virtual void resize(std::size_t width, std::size_t height) override;

private:
	GUI::TexturedBox box{getImageCache(), ":resources/widget.png",
			0, 0, // atlas offset (x, y)
			7, 1, 7, // dx1, dx2, dx3
			7, 63, 7}; // dy1, dy2, dy3

	class Canvas
		: public GUI::Widget
	{
	public:
		Canvas(GUI::Widget* parent, Settings& settings,
		       SettingsNotifier& settings_notifier);

		// From Widget:
		virtual void repaintEvent(GUI::RepaintEvent *repaintEvent) override;

		void latencyEnabledChanged(bool enabled);
		void velocityEnabledChanged(bool enabled);
		void latencyOffsetChanged(float offset);
		void velocityOffsetChanged(float offset);
		void latencyStddevChanged(float stddev);
		void latencyLaidbackChanged(float laidback);
		void velocityStddevChanged(float stddev);

		GUI::Texture stddev_h{getImageCache(), ":resources/stddev_horizontal.png"};
		GUI::Texture stddev_h_disabled{getImageCache(), ":resources/stddev_horizontal_disabled.png"};
		GUI::Texture stddev_v{getImageCache(), ":resources/stddev_vertical.png"};
		GUI::Texture stddev_v_disabled{getImageCache(), ":resources/stddev_vertical_disabled.png"};

		bool latency_enabled{false};
		bool velocity_enabled{false};

		float latency_offset;
		float velocity_offset;
		float latency_stddev;
		int laidback;
		float velocity_stddev;
		SettingsNotifier& settings_notifier;
		const float latency_max_ms;

		Settings& settings;
	};

	Canvas canvas;
};

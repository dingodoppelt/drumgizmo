/* -*- Mode: c++ -*- */
/***************************************************************************
 *            powerwidget.cc
 *
 *  Fri Apr 24 17:30:45 CEST 2020
 *  Copyright 2020 Bent Bisballe Nyeng
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
#include "powerwidget.h"

#include <dggui/painter.h>
#include <dggui/colour.h>

#include <notifier.h>
#include <settings.h>
#include <powermap.h>

#include <hugin.hpp>
#include <cmath>

#include <translation.h>

namespace GUI
{

PowerWidget::PowerWidget(dggui::Widget* parent,
                         Settings& settings,
                         SettingsNotifier& settings_notifier)
	: dggui::Widget(parent)
	, canvas(this, settings, settings_notifier)
	, settings(settings)
{
	canvas.move(7, 7);

	CONNECT(&shelf_checkbox, stateChangedNotifier, this, &PowerWidget::chk_shelf);

	shelf_label.setText(_("Shelf"));
	shelf_label.setAlignment(dggui::TextAlignment::center);
	shelf_label.resize(59, 16);
	shelf_checkbox.resize(59, 40);

	CONNECT(&settings_notifier, powermap_shelf, &shelf_checkbox,
	        &dggui::CheckBox::setChecked);
}

void PowerWidget::chk_shelf(bool v)
{
	settings.powermap_shelf.store(v);
}

void PowerWidget::repaintEvent(dggui::RepaintEvent *repaintEvent)
{
	dggui::Painter p(*this);
	box.setSize(width() - 59 - 64, height());
	p.drawImage(0, 0, box);
}

void PowerWidget::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);
	if(width < (14 + 59 + 64) || height < 14)
	{
		canvas.resize(1, 1);
		return;
	}
	canvas.resize(width - 14 - 59 - 64, height - 14);

	shelf_label.move(width - 59 + 5 - 32 , 0);
	shelf_checkbox.move(width - 59 + 5 - 32, 16);
}

PowerWidget::Canvas::Canvas(dggui::Widget* parent,
                            Settings& settings,
                            SettingsNotifier& settings_notifier)
	: dggui::Widget(parent)
	, settings_notifier(settings_notifier)
	, settings(settings)
{
	CONNECT(this, settings_notifier.enable_powermap,
	        this, &PowerWidget::Canvas::parameterChangedBool);
	CONNECT(this, settings_notifier.powermap_fixed0_x,
	        this, &PowerWidget::Canvas::parameterChangedFloat);
	CONNECT(this, settings_notifier.powermap_fixed0_y,
	        this, &PowerWidget::Canvas::parameterChangedFloat);
	CONNECT(this, settings_notifier.powermap_fixed1_x,
	        this, &PowerWidget::Canvas::parameterChangedFloat);
	CONNECT(this, settings_notifier.powermap_fixed1_y,
	        this, &PowerWidget::Canvas::parameterChangedFloat);
	CONNECT(this, settings_notifier.powermap_fixed2_x,
	        this, &PowerWidget::Canvas::parameterChangedFloat);
	CONNECT(this, settings_notifier.powermap_fixed2_y,
	        this, &PowerWidget::Canvas::parameterChangedFloat);
	CONNECT(this, settings_notifier.powermap_shelf,
	        this, &PowerWidget::Canvas::parameterChangedBool);
	CONNECT(this, settings_notifier.powermap_input,
	        this, &PowerWidget::Canvas::parameterChangedFloat);
	CONNECT(this, settings_notifier.powermap_output,
	        this, &PowerWidget::Canvas::parameterChangedFloat);

	parameterChangedFloat(0);
}

void PowerWidget::Canvas::repaintEvent(dggui::RepaintEvent *repaintEvent)
{
	if(width() < 1 || height() < 1)
	{
		return;
	}

	const float x0 = brd;
	const float y0 = brd;
	const float width0 = (int)width() - 2 * brd;
	const float height0 = (int)height() - 2 * brd;

	dggui::Painter p(*this);

	p.clear();

	p.setColour(dggui::Colour(1.0f, 1.0f, 1.0f, 0.2f));
	p.drawRectangle(x0, y0 + height0, x0 + width0, y0);

	if(enabled)
	{
		// draw 1:1 line in grey in the background to indicate where 1:1 is
		p.setColour(dggui::Colour(0.5));
		p.drawLine(x0, y0 + height0, x0 + width0, y0);
	}

	if(enabled)
	{
		// enabled green
		p.setColour(dggui::Colour(0.0f, 1.0f, 0.0f, 1.0f));
	}
	else
	{
		// disabled grey
		p.setColour(dggui::Colour(0.5f));
	}

	// Draw very short line segments across the region
	std::pair<int, int> old{};
	for(std::size_t x = 0; x < width0; ++x)
	{
		int y = power_map.map((float)x / width0) * height0;
		if(x > 0)
		{
			p.drawLine(x0 + old.first, y0 + old.second, x0 + x, y0 + height0 - y);
		}
		old = { x, height0 - y };
	}

	int x = width0;
	int y = power_map.map((float)x / width0) * height0;
	p.drawLine(x0 + old.first, y0 + old.second, x0 + x, y0 + height0 - y);
	old = { x, height0 - y };

	if(!enabled)
	{
		// draw 1:1 line in green in the foreground
		p.setColour(dggui::Colour(0.0f, 1.0f, 0.0f, 1.0f));
		p.drawLine(x0, y0 + height0, x0 + width0, y0);
	}

	// draw the input/output of the last hit
	if(settings.powermap_input.load() != -1 && settings.powermap_output.load() != -1)
	{
		p.setColour(dggui::Colour(.8f, 0.0f, .2f, .5f));
		p.drawLine(x0 + settings.powermap_input.load()*width0, y0 + height0,
				   x0 + settings.powermap_input.load()*width0, y0);
		p.drawLine(x0, y0 + height0 - settings.powermap_output.load()*height0,
				   x0 + width0, y0 + height0 - settings.powermap_output.load()*height0);
	}

	// draw the fixed nodes of the spline
	float rad = radius * width();
	p.setColour(dggui::Colour{0.0f, 1.0f, 0.0f, 0.7f});
	p.drawFilledCircle(x0 + std::round(settings.powermap_fixed0_x.load() * width0),
	                   y0 + height0 - std::round(settings.powermap_fixed0_y.load() * height0), rad);
	p.drawCircle(x0 + std::round(power_map.getFixed0().in * width0),
	             y0 + height0 - std::round(power_map.getFixed0().out * height0), rad + 1);

	p.setColour(dggui::Colour{1.0f, 1.0f, 0.0f, 0.7f});
	p.drawFilledCircle(x0 + std::round(settings.powermap_fixed1_x.load() * width0),
	                   y0 + height0 - std::round(settings.powermap_fixed1_y.load() * height0), rad);
	p.drawCircle(x0 + std::round(power_map.getFixed1().in * width0),
	             y0 + height0 - std::round(power_map.getFixed1().out * height0), rad + 1);

	p.setColour(dggui::Colour{1.0f, 0.0f, 0.0f, 0.7f});
	p.drawFilledCircle(x0 + std::round(settings.powermap_fixed2_x.load() * width0),
	                   y0 + height0 - std::round(settings.powermap_fixed2_y.load() * height0), rad);
	p.drawCircle(x0 + std::round(power_map.getFixed2().in * width0),
	             y0 + height0 - std::round(power_map.getFixed2().out * height0), rad + 1);

	p.setColour(dggui::Colour(1.0f, 1.0f, 1.0f, 0.2f));
	p.drawText(width() / 2 - (font.textWidth(_("in")) / 2), height() - 8, font, _("in"));
	p.drawText(8, height() / 2 - (font.textWidth(_("out")) / 2), font, _("out"), false, true);
}

void PowerWidget::Canvas::buttonEvent(dggui::ButtonEvent* buttonEvent)
{
	const float x0 = brd;
	const float y0 = brd;
	const float width0 = (int)width() - 2 * brd;
	const float height0 = (int)height() - 2 * brd;

	float mx0 = (float)(buttonEvent->x - x0) / width0;
	float my0 = (float)(((int)height() - buttonEvent->y) - y0) / height0;

	float radius_x = radius * 2;
	float radius_y = radius * width0 / height0 * 2;

	switch(buttonEvent->direction)
	{
	case dggui::Direction::up:
		in_point = -1;
		break;
	case dggui::Direction::down:
		if(std::abs(mx0 - settings.powermap_fixed0_x.load()) < radius_x &&
		   std::abs(my0 - settings.powermap_fixed0_y.load()) < radius_y)
		{
			in_point = 0;
		}

		if(std::abs(mx0 - settings.powermap_fixed1_x.load()) < radius_x &&
		   std::abs(my0 - settings.powermap_fixed1_y.load()) < radius_y)
		{
			in_point = 1;
		}

		if(std::abs(mx0 - settings.powermap_fixed2_x.load()) < radius_x &&
		   std::abs(my0 - settings.powermap_fixed2_y.load()) < radius_y)
		{
			in_point = 2;
		}
		break;
	}
}

namespace
{
float clamp(float val, float min, float max)
{
	return std::max(min, std::min(max, val));
}
}

void PowerWidget::Canvas::mouseMoveEvent(dggui::MouseMoveEvent* mouseMoveEvent)
{
	const float x0 = brd;
	const float y0 = brd;
	const float width0 = (int)width() - 2 * brd;
	const float height0 = (int)height() - 2 * brd;

	float mx0 = (float)(mouseMoveEvent->x - x0) / width0;
	float my0 = (float)(((int)height() - mouseMoveEvent->y) - y0) / height0;

	switch(in_point)
	{
	case 0:
		settings.powermap_fixed0_x.store(clamp(mx0, 0, 1));
		settings.powermap_fixed0_y.store(clamp(my0, 0, 1));
		redraw();
		break;
	case 1:
		settings.powermap_fixed1_x.store(clamp(mx0, 0, 1));
		settings.powermap_fixed1_y.store(clamp(my0, 0, 1));
		redraw();
		break;
	case 2:
		settings.powermap_fixed2_x.store(clamp(mx0, 0, 1));
		settings.powermap_fixed2_y.store(clamp(my0, 0, 1));
		redraw();
		break;
	default:
		break;
	}
/*
	switch(in_point)
	{
	case 0:
		settings.fixed0_x.store(clamp((float)mouseMoveEvent->x / width()));
		settings.fixed0_y.store(1.0f - clamp((float)mouseMoveEvent->y / height()));
		redraw();
		break;
	case 1:
		settings.fixed1_x.store(clamp((float)mouseMoveEvent->x / width()));
		settings.fixed1_y.store(1.0f - clamp((float)mouseMoveEvent->y / height()));
		redraw();
		break;
	case 2:
		settings.fixed2_x.store(clamp((float)mouseMoveEvent->x / width()));
		settings.fixed2_y.store(1.0f - clamp((float)mouseMoveEvent->y / height()));
		redraw();
		break;
	default:
		break;
	}
*/
}

void PowerWidget::Canvas::mouseLeaveEvent()
{
	//in_point = -1;
}

void PowerWidget::Canvas::parameterChangedFloat(float)
{
	power_map.setFixed0({settings.powermap_fixed0_x.load(), settings.powermap_fixed0_y.load()});
	power_map.setFixed1({settings.powermap_fixed1_x.load(), settings.powermap_fixed1_y.load()});
	power_map.setFixed2({settings.powermap_fixed2_x.load(), settings.powermap_fixed2_y.load()});
	power_map.setShelf(settings.powermap_shelf.load());
	enabled = settings.enable_powermap.load();
	redraw();
}

void PowerWidget::Canvas::parameterChangedBool(bool)
{
	parameterChangedFloat(0);
}

} // ::GUI

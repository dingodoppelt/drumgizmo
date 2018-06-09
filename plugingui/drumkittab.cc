/* -*- Mode: c++ -*- */
/***************************************************************************
 *            drumkittab.cc
 *
 *  Fri Jun  8 21:50:03 CEST 2018
 *  Copyright 2018 André Nusser
 *  andre.nusser@googlemail.com
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
#include "drumkittab.h"

#include <iomanip>
#include <sstream>

#include "cpp11fix.h" // required for c++11
#include "painter.h"
#include "settings.h"

namespace GUI
{

DrumkitTab::DrumkitTab(Widget* parent,
                       Settings& settings/*,
                       SettingsNotifier& settings_notifier,
                       Config& config*/)
	: Widget(parent)
	, settings(settings)
	/*, settings_notifier(settings_notifier)
	, config(config)*/
{
	loadImageFiles("/home/chaot/Programming/drumgizmo/crocellkit01.png", "/home/chaot/Programming/drumgizmo/crocellkit01_map.png");

	velocity_label.move(10, height()-velocity_label.height()-5);
	updateVelocityLabel();
	velocity_label.resizeToText();
}

void DrumkitTab::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);

	if (drumkit_image) {
		Painter painter(*this);
		painter.clear();
		painter.drawImage(0 , 0, *drumkit_image);
	}

	velocity_label.move(10, height-velocity_label.height()-5);
}

void DrumkitTab::buttonEvent(ButtonEvent* buttonEvent)
{
	if (buttonEvent->button == MouseButton::left &&
	    buttonEvent->direction == GUI::Direction::down)
	{
		triggerAudition(buttonEvent->x, buttonEvent->y);
	}
}

void DrumkitTab::scrollEvent(ScrollEvent* scrollEvent)
{
	current_velocity -=  0.01*scrollEvent->delta;
	current_velocity = std::max(std::min(current_velocity, 1.0f), 0.0f);
	updateVelocityLabel();
	velocity_label.resizeToText();

	triggerAudition(scrollEvent->x, scrollEvent->y);
}

void DrumkitTab::triggerAudition(int x, int y)
{
	auto map_colour = map_image->getPixel(x, y);

	auto it = colour_to_instrument.find(map_colour);
	if (it == colour_to_instrument.end())
	{
		return;
	}

	++settings.audition_counter;
	settings.audition_instrument = it->second;
	settings.audition_velocity = current_velocity;
}

void DrumkitTab::updateVelocityLabel()
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << current_velocity;
	velocity_label.setText("Velocity: " + stream.str());
}

void DrumkitTab::loadImageFiles(std::string const& image_file, std::string const& map_file)
{
	drumkit_image = std::make_unique<Image>(image_file);
	map_image = std::make_unique<Image>(map_file);

	// TODO: actually use mapping from drumkit file here
	colour_to_instrument = {
		{Colour(0), "Snare"},
		{Colour(255./255, 15./255, 55./255), "KdrumL"},
		{Colour(154./255, 153./255, 33./255), "HihatClosed"},
		{Colour(248./255, 221./255, 37./255), "Tom4"}
	};
}

} // GUI::

/* -*- Mode: c++ -*- */
/***************************************************************************
 *            drumkittab.h
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
#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include <grid.h>

#include "image.h"
#include "label.h"
#include "widget.h"

struct Settings;
class SettingsNotifier;

namespace GUI
{

class Config;

class DrumkitTab
	: public Widget
{
public:
	DrumkitTab(Widget* parent,
	           Settings& settings,
	           SettingsNotifier& settings_notifier/*,
	           Config& config*/);

	// From Widget:
	void resize(std::size_t width, std::size_t height) override;
	void buttonEvent(ButtonEvent* buttonEvent) override;
	void scrollEvent(ScrollEvent* scrollEvent) override;
	void mouseMoveEvent(MouseMoveEvent* mouseMoveEvent) override;
	void mouseLeaveEvent() override;

	void init(std::string const& image_file, std::string const& map_file);

private:
	float current_velocity{.5};
	std::string current_instrument{""};
	int current_index{-1};

	using IndexGrid = Grid<int>;
	using Position = IndexGrid::Pos;
	using Positions = std::vector<Position>;

	std::vector<Colour> colours;
	IndexGrid pos_to_colour_index;
	std::vector<Positions> colour_index_to_positions;
	std::vector<std::string> to_instrument_name;

	struct ColourInstrumentPair
	{
		Colour colour;
		std::string instrument;
	};
	// FIXME: load this from instrument file
	std::vector<ColourInstrumentPair> colour_instrument_pairs = {
		{Colour(0), "Snare"},
		{Colour(255./255, 15./255, 55./255), "KdrumL"},
		{Colour(154./255, 153./255, 33./255), "HihatClosed"},
		{Colour(248./255, 221./255, 37./255), "Tom4"}
	};

	bool shows_overlay{false};
	bool shows_instrument_overlay{false};

	std::unique_ptr<Image> drumkit_image;
	std::unique_ptr<Image> map_image;
	int drumkit_image_x;
	int drumkit_image_y;

	Label velocity_label{this};
	Label instrument_name_label{this};

	Settings& settings;
	SettingsNotifier& settings_notifier;
	// Config& config;

	void triggerAudition(int x, int y);
	void highlightInstrument(int index);
	void updateVelocityLabel();
	void updateInstrumentLabel(int index);

	void drumkitFileChanged(const std::string& drumkit_file);
};

} // GUI::

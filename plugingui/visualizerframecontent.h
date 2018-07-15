/* -*- Mode: c++ -*- */
/***************************************************************************
 *            visualizerframecontent.h
 *
 *  Tue Jul 10 20:52:22 CEST 2018
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
#include "layout.h"
#include "widget.h"
#include "humaniservisualiser.h"

#include <settings.h>

#include <iomanip>
#include <sstream>

class SettingsNotifier;

namespace GUI
{

class VisualizerframeContent
	: public Widget
{
public:
	VisualizerframeContent(Widget* parent, Settings& settings,
	                       SettingsNotifier& settings_notifier);

		// From Widget
	virtual void resize(std::size_t width, std::size_t height) override;

private:
	HumaniserVisualiser visualizer;
};

} // GUI::

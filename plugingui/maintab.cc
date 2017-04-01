/* -*- Mode: c++ -*- */
/***************************************************************************
 *            maintab.cc
 *
 *  Fri Mar 24 20:39:59 CET 2017
 *  Copyright 2017 André Nusser
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
#include "maintab.h"

namespace GUI
{

MainTab::MainTab(Widget* parent) : Widget(parent)
{
	layout.setSpacing(10);
	layout.setResizeChildren(true);
	layout.setHAlignment(GUI::HAlignment::center);

	layout.addItem(&drumkit_frame);
	layout.addItem(&status_frame);
	layout.addItem(&humanizer_frame);
	layout.addItem(&diskstreaming_frame);

	drumkit_frame.setTitle("Drumkit");
	status_frame.setTitle("Status");
	humanizer_frame.setTitle("Humanizer");
	diskstreaming_frame.setTitle("Disk streaming");

	drumkit_frame.setContent(&drumkitframe_content);
	status_frame.setContent(&statusframe_content);
	humanizer_frame.setContent(&humanizerframe_content);
	diskstreaming_frame.setContent(&diskstreamingframe_content);
}

} // GUI::

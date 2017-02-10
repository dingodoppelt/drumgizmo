/* -*- Mode: c++ -*- */
/***************************************************************************
 *            diskstreamingframecontent.cc
 *
 *  Fri Mar 24 21:50:07 CET 2017
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
#include "diskstreamingframecontent.h"

namespace GUI {

DiskstreamingframeContent::DiskstreamingframeContent(Widget* parent)
	: Widget(parent)
	, slider_width{250}
{
	label_text.setText("Cache limit (max memory usage):");
	label_text.setAlignment(TextAlignment::center);

	button.setText("Apply"); // TODO connect

	label_size.setText("600 MB"); // TODO connect
	label_size.setAlignment(TextAlignment::center);
}

void DiskstreamingframeContent::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);

	slider_width = 0.8 * width;
	button_width = width - slider_width;

	label_text.move(0,0);
	slider.move(0, 20);
	button.move(slider_width, 10);
	label_size.move(0, 40);

	label_text.resize(slider_width, 15);
	slider.resize(slider_width, 15);
	button.resize(button_width, 30);
	label_size.resize(slider_width, 15);
}

} // GUI::

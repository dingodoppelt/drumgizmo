/* -*- Mode: c++ -*- */
/***************************************************************************
 *            drumkitframecontent.cc
 *
 *  Fri Mar 24 21:49:42 CET 2017
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
#include "drumkitframecontent.h"

#include "label.h"

namespace GUI
{

DrumkitframeContent::DrumkitframeContent(Widget* parent) : Widget(parent)
{
	layout.setHAlignment(HAlignment::left);

	drumkitCaption.setText("Drumkit file:");
	midimapCaption.setText("Midimap file:");

	layout.addItem(&drumkitCaption);
	layout.addItem(&drumkitFile);
	layout.addItem(&drumkitFileProgress);
	layout.addItem(&midimapCaption);
	layout.addItem(&midimapFile);
	layout.addItem(&midimapFileProgress);

	// TODO:
	// CONNECT(&drumkitFile.browseButton, clickNotifier,
	//         this, &DGWindow::kitBrowseClick);
	// TODO:
	// CONNECT(&midimapFile.browseButton, clickNotifier,
	//         this, &DGWindow::midimapBrowseClick);

	midimapFileProgress.setTotal(2);
}

void DrumkitframeContent::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);

	drumkitCaption.resize(width, 15);
	drumkitFile.resize(width, 37);
	drumkitFileProgress.resize(drumkitFile.getLineEditWidth(), 11);

	midimapCaption.resize(width, 15);
	midimapFile.resize(width, 37);
	midimapFileProgress.resize(drumkitFile.getLineEditWidth(), 11);

	layout.layout();
}

} // GUI::

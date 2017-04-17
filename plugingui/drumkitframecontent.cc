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

File::File(Widget* parent)
	: Widget(parent)
{
	layout.setResizeChildren(false);
	layout.setVAlignment(VAlignment::center);

	layout.addItem(&lineedit);
	layout.addItem(&browse_button);

	browse_button.setText("Browse...");
}

void File::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);

	lineedit_width = 0.72 * width;
	button_width = width - lineedit_width;

	lineedit.resize(lineedit_width, 29);
	browse_button.resize(button_width, 30);

	layout.layout();
}

std::size_t File::getLineEditWidth()
{
	return lineedit_width;
}

std::size_t File::getButtonWidth()
{
	return button_width;
}

Button& File::getBrowseButton()
{
	return browse_button;
}

LineEdit& File::getLineEdit()
{
	return lineedit;
}

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

	CONNECT(&drumkitFile.getBrowseButton(), clickNotifier,
	        this, &DrumkitframeContent::kitBrowseClick);
	CONNECT(&midimapFile.getBrowseButton(), clickNotifier,
	        this, &DrumkitframeContent::midimapBrowseClick);

	midimapFileProgress.setTotal(2);

	file_browser.resize(450, 350);
	file_browser.setFixedSize(450, 350);
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

void DrumkitframeContent::kitBrowseClick()
{
	CONNECT(&file_browser, fileSelectNotifier,
	        this, &DrumkitframeContent::selectKitFile);
	file_browser.show();
}

void DrumkitframeContent::midimapBrowseClick()
{
	CONNECT(&file_browser, fileSelectNotifier,
	        this, &DrumkitframeContent::selectMapFile);
	file_browser.show();
}

void DrumkitframeContent::selectKitFile(const std::string& filename)
{
	auto& line_edit = drumkitFile.getLineEdit();
	line_edit.setText(filename);

	// TODO:
	//config.lastkit = drumkit;
	//config.save();

	// TODO:
	//settings.drumkit_file.store(drumkit);
}

void DrumkitframeContent::selectMapFile(const std::string& filename)
{
	auto& line_edit = midimapFile.getLineEdit();
	line_edit.setText(filename);

	// TODO:
	//config.lastmidimap = midimap;
	//config.save();

	// TODO:
	//settings.midimap_file.store(midimap);
}

} // GUI::

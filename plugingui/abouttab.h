/* -*- Mode: c++ -*- */
/***************************************************************************
 *            abouttab.h
 *
 *  Fri Apr 21 18:51:13 CEST 2017
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
#pragma once

#include <dggui/widget.h>
#include <dggui/resource.h>
#include <dggui/textedit.h>

#include <string>

namespace GUI
{

class AboutTab
	: public dggui::Widget
{
public:
	AboutTab(dggui::Widget* parent);

	// From Widget:
	void resize(std::size_t width, std::size_t height) override;

private:
	std::string getAboutText();

	dggui::TextEdit text_edit{this};
	int margin{10};
};

} // GUI::

/* -*- Mode: c++ -*- */
/***************************************************************************
 *            statusframecontent.cc
 *
 *  Fri Mar 24 21:49:50 CET 2017
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
#include "statusframecontent.h"

namespace GUI {

StatusframeContent::StatusframeContent(Widget* parent)
	: Widget(parent)
{
	text_field.move(0, 0);
	text_field.setText("This is a status message.");
	text_field.setReadOnly(true);
	text_field.show();
}

void StatusframeContent::resize(std::size_t width, std::size_t height)
{
	Widget::resize(width, height);
	text_field.resize(width, height);
}

} // GUI::

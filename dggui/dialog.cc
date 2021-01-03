/* -*- Mode: c++ -*- */
/***************************************************************************
 *            dialog.cc
 *
 *  Sun Apr 16 10:31:04 CEST 2017
 *  Copyright 2017 Bent Bisballe Nyeng
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
#include "dialog.h"

namespace dggui
{

Dialog::Dialog(Widget* parent, bool modal)
	: parent(parent)
{
	parent->window()->eventHandler()->registerDialog(this);
	setModal(modal);
}

Dialog::~Dialog()
{
	parent->window()->eventHandler()->unregisterDialog(this);
}

void Dialog::setModal(bool modal)
{
	is_modal = modal;
}

bool Dialog::isModal() const
{
	return is_modal;
}

} // dggui::

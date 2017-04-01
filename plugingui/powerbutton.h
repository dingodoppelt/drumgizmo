/* -*- Mode: c++ -*- */
/***************************************************************************
 *            powerbutton.h
 *
 *  Thu Mar 23 12:30:50 CET 2017
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

#include "texture.h"
#include "toggle.h"

namespace GUI
{

class PowerButton : public Toggle
{
public:
	PowerButton(Widget* parent);
	virtual ~PowerButton() = default;

protected:
	// From Widget:
	virtual void repaintEvent(RepaintEvent* repaintEvent) override;

private:
	Texture on{getImageCache(), ":bypass_button.png", 0, 0, 16, 16};
	Texture on_clicked{getImageCache(), ":bypass_button.png", 16, 0, 16, 16};
	Texture off{getImageCache(), ":bypass_button.png", 32, 0, 16, 16};
	Texture off_clicked{getImageCache(), ":bypass_button.png", 48, 0, 16, 16};
};

} // GUI::

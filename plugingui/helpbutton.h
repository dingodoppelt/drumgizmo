/* -*- Mode: c++ -*- */
/***************************************************************************
 *            helpbutton.h
 *
 *  Wed May  8 17:10:08 CEST 2019
 *  Copyright 2019 Bent Bisballe Nyeng
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

#include "texture.h"
#include "button_base.h"
#include "tooltip.h"

namespace GUI
{

class HelpButton
	: public ButtonBase
{
public:
	HelpButton(Widget* parent);
	virtual ~HelpButton() = default;

	void setHelpText(const std::string& help_text);

protected:
	// From Widget:
	virtual void repaintEvent(RepaintEvent* repaintEvent) override;

private:
	void showHelpText();

	Texture normal{getImageCache(), ":resources/help_button.png", 0, 0, 16, 16};
	Texture pushed{getImageCache(), ":resources/help_button.png", 16, 0, 16, 16};

	Tooltip tip;
};

} // GUI::

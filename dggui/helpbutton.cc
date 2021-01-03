/* -*- Mode: c++ -*- */
/***************************************************************************
 *            helpbutton.cc
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
#include "helpbutton.h"

#include "painter.h"

#include <iostream>

namespace dggui
{

HelpButton::HelpButton(Widget* parent)
	: ButtonBase(parent)
	, tip(this)
{
	CONNECT(this, clickNotifier, this, &HelpButton::showHelpText);
	tip.hide();
}

void HelpButton::setHelpText(const std::string& help_text)
{
	tip.setText(help_text);
}

void HelpButton::repaintEvent(RepaintEvent* repaintEvent)
{
	Painter p(*this);

	bool state = true;

	// enabled and on
	if(state)
	{
		if(button_state == ButtonBase::State::Down)
		{
			p.drawImage(0, 0, pushed);
		}
		else
		{
			p.drawImage(0, 0, normal);
		}
		return;
	}
}

void HelpButton::showHelpText()
{
	tip.show();
}

} // dggui::

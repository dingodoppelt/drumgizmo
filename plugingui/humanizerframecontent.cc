/* -*- Mode: c++ -*- */
/***************************************************************************
 *            humanizerframecontent.cc
 *
 *  Fri Mar 24 21:49:58 CET 2017
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
#include "humanizerframecontent.h"

#include "painter.h"

namespace GUI
{

HumanizerframeContent::HumanizerframeContent(Widget* parent) : Widget(parent)
{
	// FIXME, is resizeChildren broken?
	layout.setResizeChildren(true);
	layout.setVAlignment(VAlignment::center);

	attack.resize(80, 80);
	attackKnob.resize(60, 60);
	attack.setControl(&attackKnob);
	layout.addItem(&attack);

	falloff.resize(80, 80);
	falloffKnob.resize(60, 60);
	falloff.setControl(&falloffKnob);
	layout.addItem(&falloff);

	// TODO: connect the knobs to the right functions
	// CONNECT(&humanizeControls->velocityCheck, stateChangedNotifier,
	//         this, &DGWindow::velocityCheckClick);
	//
	// CONNECT(&humanizeControls->attackKnob, valueChangedNotifier,
	//         this, &DGWindow::attackValueChanged);
	//
	// CONNECT(&humanizeControls->falloffKnob, valueChangedNotifier,
	//         this, &DGWindow::falloffValueChanged);
}

} // GUI::

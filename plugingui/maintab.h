/* -*- Mode: c++ -*- */
/***************************************************************************
 *            maintab.h
 *
 *  Fri Mar 24 20:39:59 CET 2017
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

#include "widget.h"
#include "layout.h"
#include "frame.h"
#include "drumkitframecontent.h"
#include "statusframecontent.h"
#include "humanizerframecontent.h"
#include "diskstreamingframecontent.h"

namespace GUI {

class MainTab : public Widget
{
public:
	MainTab(Widget* parent);

private:
	GridLayout layout{this, 2, 128};

	FrameWidget drumkit_frame{this, false};
	FrameWidget status_frame{this, false};
	FrameWidget humanizer_frame{this, true};
	FrameWidget diskstreaming_frame{this, true};

	DrumkitframeContent drumkitframe_content{this};
	StatusframeContent statusframe_content{this};
	HumanizerframeContent humanizerframe_content{this};
	DiskstreamingframeContent diskstreamingframe_content{this};
};

} // GUI::

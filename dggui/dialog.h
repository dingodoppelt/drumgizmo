/* -*- Mode: c++ -*- */
/***************************************************************************
 *            dialog.h
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
#pragma once

#include "window.h"

namespace dggui
{

//! This class is used the base window for pop-up dialogs, such as a file
//! browser.
class Dialog
	: public Window
{
public:
	//! - The dialog is placed near the parent window.
	//! - The parent window event handler will call the dialog event handler
	//! - While the dialog is visible, all mouse click and keyboard events
	//!   are ignored by the parent event handler.
	//! - The Dialog registers itself in the parent event handler when contructed
	//!   and removes itself when destructed.
	//! - The parent event handler will delete all registered Dialogs when itself
	//!   deleted.
	Dialog(Widget* parent, bool modal = false);

	~Dialog();

	//! Change modality.
	void setModal(bool modal);

	//! Get current modality state.
	bool isModal() const;

private:
	bool is_modal{false};
	Widget* parent{nullptr};
};

} // dggui::

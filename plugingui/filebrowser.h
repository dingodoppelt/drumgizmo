/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            filebrowser.h
 *
 *  Mon Feb 25 21:09:43 CET 2013
 *  Copyright 2013 Bent Bisballe Nyeng
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

#include <notifier.h>

#include "widget.h"
#include "button.h"
#include "listbox.h"
#include "lineedit.h"
#include "label.h"
#include "image.h"
#include "directory.h"

namespace GUI {

class FileBrowser : public Widget {
public:
	FileBrowser(Widget *parent);
	~FileBrowser();

	void setPath(const std::string& path);

	Notifier<const std::string&> fileSelectNotifier; // (const std::string& path)

	// From Widget:
	bool isFocusable() override { return true; }
	virtual void repaintEvent(RepaintEvent* repaintEvent) override;
	virtual void resize(int w, int h) override;

private:
	void listSelectionChanged();
	void selectButtonClicked();
	void cancelButtonClicked();
	void handleKeyEvent();

	Directory dir;
#ifdef WIN32
	bool above_root;
	bool in_root;
#endif

	void cancel();
	void changeDir();

	Label lbl_path;

	LineEdit lineedit;
	ListBox listbox;

	Button btn_sel;
	Button btn_esc;

	Image back;
};

} // GUI::

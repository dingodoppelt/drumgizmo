/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            filebrowser.cc
 *
 *  Mon Feb 25 21:09:44 CET 2013
 *  Copyright 2013 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "filebrowser.h"

#include "painter.h"
#include "button.h"

#include "directory.h"

#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <hugin.hpp>

#ifdef __MINGW32__
#include <direct.h>
#endif

namespace GUI {

FileBrowser::FileBrowser(Widget *parent)
	: Widget(parent)
	, dir(Directory::cwd())
	, lbl_path(this)
	, lineedit(this)
	, listbox(this)
	, btn_sel(this)
	, btn_esc(this)
	, back(":bg.png")
{
#ifdef WIN32
	above_root = false;
#endif

	lbl_path.setText("Path:");

	//lineedit.setReadOnly(true);
	CONNECT(&lineedit, enterPressedNotifier, this, &FileBrowser::handleKeyEvent);
	CONNECT(&listbox, selectionNotifier,
	        this, &FileBrowser::listSelectionChanged);

	btn_sel.setText("Select");
	CONNECT(&btn_sel, clickNotifier, this, &FileBrowser::selectButtonClicked);

	btn_esc.setText("Cancel");
	CONNECT(&btn_esc, clickNotifier, this, &FileBrowser::cancelButtonClicked);

	changeDir();
}

FileBrowser::~FileBrowser()
{
}

void FileBrowser::setPath(const std::string& path)
{
	INFO(filebrowser, "Setting path to '%s'\n", path.c_str());

	if(!path.empty())
	{
		dir.setPath(Directory::pathDirectory(path));
	}
	else
	{
		dir.setPath(Directory::pathDirectory(Directory::cwd()));
	}

	listbox.clear();

	changeDir();
}

void FileBrowser::resize(int w, int h)
{
	Widget::resize(w,h);

	int offset = 0;
	int brd = 5; // border
	int btn_h = 30;

	offset += brd;

	lbl_path.move(0, offset);
	lineedit.move(60, offset);

	offset += btn_h;

	lbl_path.resize(60, btn_h);
	lineedit.resize(w - 60 - brd, btn_h);

	offset += brd;

	listbox.move(brd, offset);
	listbox.resize(w - 1 - 2*brd, h - btn_h - 2*brd - offset);

	btn_esc.move(brd, h - btn_h - brd);
	btn_esc.resize((w - 1 - 2*brd) / 2 - brd / 2, btn_h);

	btn_sel.move(brd + w / 2 - brd / 2, h - btn_h - brd);
	btn_sel.resize((w - 1 - 2*brd) / 2, btn_h);
}

void FileBrowser::repaintEvent(RepaintEvent *e)
{
	Painter p(*this);
	p.drawImageStretched(0,0, back, width(), height());
}

void FileBrowser::listSelectionChanged()
{
	changeDir();
}

void FileBrowser::selectButtonClicked()
{
	changeDir();
}

void FileBrowser::cancelButtonClicked()
{
	cancel();
}

void FileBrowser::handleKeyEvent()
{
	listbox.clearSelectedValue();

	std::string value = lineedit.text();
	if((value.size() > 1) && (value[0] == '@'))
	{
		DEBUG(filebrowser, "Selecting ref-file '%s'\n", value.c_str());
		fileSelectNotifier(value);
		return;
	}

	dir.setPath(lineedit.text());
	changeDir();
}

void FileBrowser::cancel()
{
	hide();
}

void FileBrowser::changeDir()
{
	std::string value = listbox.selectedValue();

//  if(!Directory::isDir(dir->path() + dir->seperator()))
//  {
//    return;
//  }

	listbox.clear();

	INFO(filebrowser, "Changing path to '%s'\n",
	     (dir.path() + dir.seperator() + value).c_str());

#ifdef WIN32
	if(above_root && !value.empty())
	{
		dir.setPath(value + dir.seperator());
		value.clear();
		above_root = false;
	}
#endif

	if(value.empty() && !dir.isDir() && Directory::exists(dir.path()))
	{
		DEBUG(filebrowser, "Selecting file '%s'\n", dir.path().c_str());
		fileSelectNotifier(dir.path());
		return;
	}

	if(!value.empty() && dir.fileExists(value))
	{
	  std::string file = dir.path() + dir.seperator() + value;
	  DEBUG(filebrowser, "Selecting file '%s'\n", file.c_str());
	  fileSelectNotifier(file);
	  return;
	}

	std::vector<ListBoxBasic::Item> items;

#ifdef WIN32
	if(Directory::isRoot(dir.path()) && (value == ".."))
	{
		DEBUG(filebrowser, "Showing partitions...\n");
		for(auto drive : dir.drives())
		{
			ListBoxBasic::Item item;
			item.name = drive.name;
			item.value = drive.name;
			items.push_back(item);
		}
		above_root = true;
	}
	else
#endif
	{
		if(!value.empty() && !dir.cd(value))
		{
			DEBUG(filebrowser, "Error changing to '%s'\n",
			      (dir.path() + dir.seperator() + value).c_str());
			return;
		}

		Directory::EntryList entries = dir.entryList();

		if(entries.empty())
		{
			dir.cdUp();
			entries = dir.entryList();
		}

		DEBUG(filebrowser, "Setting path of lineedit to %s\n", dir.path().c_str());
		lineedit.setText(dir.path());

		for(auto entry : entries)
		{
			ListBoxBasic::Item item;
			item.name = entry;
			item.value = entry;
			items.push_back(item);
		}
	}

	listbox.addItems(items);
}

} // GUI::

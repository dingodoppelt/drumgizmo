/* -*- Mode: c++ -*- */
/***************************************************************************
 *            abouttab.cc
 *
 *  Fri Apr 21 18:51:13 CEST 2017
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
#include "abouttab.h"

#include <version.h>
#include <translation.h>

#include <dggui/utf8.h>

namespace
{
std::string getLocalizedFile(const std::string& file)
{
#ifdef WITH_NLS
	auto language = Translation::getISO639LanguageName();
	std::string file_localized = file + "." + language;
	dggui::Resource resource_localized{file_localized};
	if(resource_localized.valid())
	{
		return resource_localized.data();
	}
#endif

	dggui::Resource resource{file};
	if(resource.valid())
	{
		return resource.data();
	}

	return "";
}
}

namespace GUI
{

AboutTab::AboutTab(dggui::Widget* parent)
	: dggui::Widget(parent)
{
	text_edit.setText(getAboutText());
	text_edit.setReadOnly(true);
	text_edit.resize(std::max((int)width() - 2*margin,0),
	                 std::max((int)height() - 2*margin, 0));
	text_edit.move(margin, margin);
}

void AboutTab::resize(std::size_t width, std::size_t height)
{
	dggui::Widget::resize(width, height);
	text_edit.resize(std::max((int)width - 2*margin, 0),
	                 std::max((int)height - 2*margin, 0));
}

std::string AboutTab::getAboutText()
{
	std::string about_text;

	// About
	about_text.append(_(
	"=============\n"
	"             About\n"
	"=============\n"
	"\n"));
	about_text.append(dggui::UTF8().toLatin1(getLocalizedFile(":../ABOUT")));

	// Version
	about_text.append(_(
	"\n"
	"=============\n"
	"            Version\n"
	"=============\n"
	"\n"));
	about_text.append(std::string(VERSION) + "\n");

	// Bugs
	about_text.append(_(
	"\n"
	"=============\n"
	"            Bugs\n"
	"=============\n"
	"\n"));
	about_text.append(dggui::UTF8().toLatin1(getLocalizedFile(":../BUGS")));

	// Authors
	about_text.append(_(
	"\n"
	"=============\n"
	"            Authors\n"
	"=============\n"
	"\n"));
	about_text.append(dggui::UTF8().toLatin1(getLocalizedFile(":../AUTHORS")));

	// GPL
	about_text.append(_(
	"\n"
	"=============\n"
	"            License\n"
	"=============\n"
	"\n"));
	about_text.append(dggui::UTF8().toLatin1(getLocalizedFile(":../COPYING")));

	return about_text;
}

} // GUI::

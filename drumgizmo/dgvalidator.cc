/* -*- Mode: c++ -*- */
/***************************************************************************
 *            dgvalidator.cc
 *
 *  Sun Jan 27 10:44:44 CET 2019
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
#include <dgxmlparser.h>
#include <path.h>
#include <domloader.h>
#include <random.h>
#include <settings.h>
#include <drumkit.h>

#include <string>
#include <hugin.hpp>

int main(int argc, char* argv[])
{
	std::string edited_filename = argv[1];
	DrumkitDOM drumkitdom;
	std::vector<InstrumentDOM> instrumentdoms;
	std::string path = getPath(edited_filename);
	bool parseerror = false;
	bool ret = parseDrumkitFile(edited_filename, drumkitdom);
	if(!ret)
	{
		WARN(drumkitloader, "Drumkit file parser error: '%s'",
		     edited_filename.data());
	}

	parseerror |= !ret;

	for(const auto& ref : drumkitdom.instruments)
	{
		instrumentdoms.emplace_back();
		bool ret = parseInstrumentFile(path + "/" + ref.file, instrumentdoms.back());
		if(!ret)
		{
			WARN(drumkitloader, "Instrument file parser error: '%s'",
			     edited_filename.data());
		}

		parseerror |= !ret;
	}

	if(parseerror)
	{
		return 1;
	}

	Settings settings;
	Random rand;
	DrumKit kit;

	DOMLoader domloader(settings, rand);
	ret = domloader.loadDom(path, drumkitdom, instrumentdoms, kit);
	if(!ret)
	{
		WARN(drumkitloader, "DOMLoader error");
		return 1;
	}
	parseerror |= !ret;
	if(parseerror)
	{
		ERR(drumgizmo, "Drumkit parser failed: %s\n", edited_filename.c_str());
		return 1;
	}

	return 0;
}

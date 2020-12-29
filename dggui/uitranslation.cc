/* -*- Mode: c++ -*- */
/***************************************************************************
 *            uitranslation.cc
 *
 *  Thu May  7 18:04:40 CEST 2020
 *  Copyright 2020 Bent Bisballe Nyeng
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
#include "uitranslation.h"

#ifdef WITH_NLS

#include "resource.h"

UITranslation::UITranslation()
{
	auto lang = Translation::getISO639LanguageName();
	printf("LANG: %s\n", lang.data());
	std::string res = ":locale/";
	res += lang + ".mo";

	GUI::Resource mo(res);
	if(!mo.valid())
	{
		printf("Locale not in resources - use default\n");
		// Locale not in resources - use default
		return;
	}
	printf("Using mo: %s\n", res.data());
	load(mo.data(), mo.size());
}

#endif // WITH_NLS

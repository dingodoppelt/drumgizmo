/* -*- Mode: c++ -*- */
/***************************************************************************
 *            translationtest.cc
 *
 *  Sun Sep  8 14:51:08 CEST 2019
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
#include "dgunit.h"

#include <locale>

#include <translation.h>
#include <uitranslation.h>
#include <stdlib.h>

class TranslationTest
	: public DGUnit
{
public:
	TranslationTest()
	{
		DGUNIT_TEST(TranslationTest::testFromFile);
		DGUNIT_TEST(TranslationTest::testFromLocale);
	}

	void testFromFile()
	{
		Translation t;
		char buf[100000];
		FILE* fp = fopen(MO_SRC, "r");
		DGUNIT_ASSERT(fp != nullptr);
		auto sz = fread(buf, 1, sizeof(buf), fp);
		fclose(fp);
		DGUNIT_ASSERT(t.load(buf, sz));

		// Look up translation from .mo file
		DGUNIT_ASSERT_EQUAL(std::string("Trommes�t"),
		                    std::string(_("Drumkit")));

		// No translation, return key
		DGUNIT_ASSERT_EQUAL(std::string("No translation"),
		                    std::string(_("No translation")));
	}

	void testFromLocale()
	{
		setenv("LANG", "da_DK.UTF-8", 1);
		UITranslation t;

		// Look up translation from .mo file
		DGUNIT_ASSERT_EQUAL(std::string("Trommes�t"),
		                    std::string(_("Drumkit")));

		// No translation, return key
		DGUNIT_ASSERT_EQUAL(std::string("No translation"),
		                    std::string(_("No translation")));
	}
};

// Registers the fixture into the 'registry'
static TranslationTest test;
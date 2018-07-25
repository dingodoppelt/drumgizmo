/* -*- Mode: c++ -*- */
/***************************************************************************
 *            configparsertest.cc
 *
 *  Wed Jul 25 19:08:40 CEST 2018
 *  Copyright 2018 Bent Bisballe Nyeng
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

#include <configparser.h>

class ConfigParserTest
	: public DGUnit
{
public:
	ConfigParserTest()
	{
		DGUNIT_TEST(ConfigParserTest::test);
	}

	void test()
	{
		std::string xml =
			"<?xml version='1.0' encoding='UTF-8'?>\n" \
			"<config>\n" \
			"  <value name=\"foo\">42</value>\n" \
			"  <value name=\"bar\">true</value>\n" \
			"  <value name=\"bas\">\"</value>\n" \
			"</config>";


		ConfigParser parser;
		DGUNIT_ASSERT(!parser.parseString(xml));

		DGUNIT_ASSERT_EQUAL(std::string("42"), parser.value("foo", "-"));
		DGUNIT_ASSERT_EQUAL(std::string("true"), parser.value("bar", "-"));
		DGUNIT_ASSERT_EQUAL(std::string("\""), parser.value("bas", "-"));

		// Non-existing value
		DGUNIT_ASSERT_EQUAL(std::string("-"), parser.value("bas2", "-"));
	}
};

// Registers the fixture into the 'registry'
static ConfigParserTest test;

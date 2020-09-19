/* -*- Mode: c++ -*- */
/***************************************************************************
 *            bytesizeparsertest.cc
 *
 *  Sun Mar 05 11:44:23 CET 2017
 *  Copyright 2017 Goran Mekić
 *  meka@tilda.center
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
#include <uunit.h>

#include "bytesizeparser.h"


class ByteSizeParserTest
	: public uUnit
{
public:
	ByteSizeParserTest()
	{
		uUNIT_TEST(ByteSizeParserTest::suffixTest);
		uUNIT_TEST(ByteSizeParserTest::falseSuffixTest);
		uUNIT_TEST(ByteSizeParserTest::falseNumberTest);
		uUNIT_TEST(ByteSizeParserTest::tooBigNumberTest);
	}

	void suffixTest()
	{
		std::size_t computed_size, expected_size;
		std::size_t kilo = 1024, mega = kilo * 1024, giga = mega * 1024;
		computed_size = byteSizeParser("3");
		expected_size = 3;
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);

		computed_size = byteSizeParser("3k");
		expected_size = 3 * kilo;
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);

		computed_size = byteSizeParser("3M");
		expected_size = 3 * mega;
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);

		computed_size = byteSizeParser("3G");
		expected_size = 3 * giga;
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);
	}

	void falseSuffixTest()
	{
		std::size_t computed_size, expected_size = 0;
		computed_size = byteSizeParser("3K");
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);

		computed_size = byteSizeParser("3m");
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);

		computed_size = byteSizeParser("3g");
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);

		computed_size = byteSizeParser("3ddDD");
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);
	}

	void falseNumberTest()
	{
		std::size_t computed_size, expected_size = 0;
		computed_size = byteSizeParser("K3k");
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);

		computed_size = byteSizeParser("-3");
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);

		computed_size = byteSizeParser("-3k");
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);

		computed_size = byteSizeParser("-3M");
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);

		computed_size = byteSizeParser("-3G");
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);

		computed_size = byteSizeParser("3-");
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);

		computed_size = byteSizeParser("3-k");
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);

		computed_size = byteSizeParser("k-3");
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);

		computed_size = byteSizeParser("3-1");
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);

		computed_size = byteSizeParser("   -3");
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);
	}

	void tooBigNumberTest()
	{
		std::size_t computed_size, expected_size = 0;
		computed_size = byteSizeParser("999999999999999999999999999999999999G");
		uUNIT_ASSERT_EQUAL(expected_size, computed_size);
	}
};

// Registers the fixture into the 'registry'
static ByteSizeParserTest test;

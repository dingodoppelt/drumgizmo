/* -*- Mode: c++ -*- */
/***************************************************************************
 *            powermaptest.cc
 *
 *  Sun Apr 19 23:23:37 CEST 2020
 *  Copyright 2020 André Nusser
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
#include "dgunit.h"

#include "../src/powermap.h"

class test_powermaptest
	: public DGUnit
{
public:
	test_powermaptest()
	{
		DGUNIT_TEST(test_powermaptest::check_values);
	}

	void check_values()
	{
		Powermap powermap;

		// TODO
		// std::cout << powermap.map(.8) << std::endl;
		// DGUNIT_ASSERT_EQUAL(powermap.map(.8), .8);
	}
};

// Registers the fixture into the 'registry'
static test_powermaptest test;

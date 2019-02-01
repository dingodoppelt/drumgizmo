/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            configtest.cc
 *
 *  Thu May 14 20:58:29 CEST 2015
 *  Copyright 2015 Bent Bisballe Nyeng
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

#include <unistd.h>
#include <stdio.h>

#include "../src/configfile.h"

class TestConfigFile
	: public ConfigFile {
public:
	TestConfigFile()
		: ConfigFile("")
	{
	}

protected:
	// Overload the built-in open method to use local file instead of homedir.
	virtual bool open(std::string mode)
	{
		fp = fopen("test.conf", mode.c_str());
		return fp != NULL;
	}
};

class test_configtest
	: public DGUnit
{
public:
	test_configtest()
	{
		DGUNIT_TEST(test_configtest::loading_no_newline);
		DGUNIT_TEST(test_configtest::loading_equal_sign);
		DGUNIT_TEST(test_configtest::loading_newline);
		DGUNIT_TEST(test_configtest::loading_padding_space);
		DGUNIT_TEST(test_configtest::loading_padding_space_newline);
		DGUNIT_TEST(test_configtest::loading_padding_tab);
		DGUNIT_TEST(test_configtest::loading_padding_tab_newline);
		DGUNIT_TEST(test_configtest::loading_comment);
		DGUNIT_TEST(test_configtest::loading_inline_comment);
		DGUNIT_TEST(test_configtest::loading_single_quoted_string);
		DGUNIT_TEST(test_configtest::loading_double_quoted_string);
		DGUNIT_TEST(test_configtest::loading_error_no_key);
		DGUNIT_TEST(test_configtest::loading_error_no_value);
		DGUNIT_TEST(test_configtest::loading_error_string_not_terminated_single);
		DGUNIT_TEST(test_configtest::loading_error_string_not_terminated_double);
		DGUNIT_TEST(test_configtest::empty_value);
	}

	void teardown() override
	{
		unlink("test.conf");
	}

	void writeFile(const char* str)
	{
		FILE* fp = fopen("test.conf", "w");
		fprintf(fp, "%s", str);
		fclose(fp);
	}

	void loading_no_newline()
	{
		writeFile("a:b");

		TestConfigFile cf;
		DGUNIT_ASSERT_EQUAL(true, cf.load());
		DGUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

	void loading_equal_sign()
	{
		writeFile(" a =\tb\t\n");

		TestConfigFile cf;
		DGUNIT_ASSERT_EQUAL(true, cf.load());
		DGUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

	void loading_newline()
	{
		writeFile("a:b\n");

		TestConfigFile cf;
		DGUNIT_ASSERT_EQUAL(true, cf.load());
		DGUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

	void loading_padding_space()
	{
		writeFile(" a : b ");

		TestConfigFile cf;
		DGUNIT_ASSERT_EQUAL(true, cf.load());
		DGUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

	void loading_padding_tab()
	{
		writeFile("\ta\t:\tb\t");

		TestConfigFile cf;
		DGUNIT_ASSERT_EQUAL(true, cf.load());
		DGUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

	void loading_padding_space_newline()
	{
		writeFile(" a : b \n");

		TestConfigFile cf;
		DGUNIT_ASSERT_EQUAL(true, cf.load());
		DGUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

	void loading_padding_tab_newline()
	{
		writeFile("\ta\t:\tb\t\n");

		TestConfigFile cf;
		DGUNIT_ASSERT_EQUAL(true, cf.load());
		DGUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

	void loading_comment()
	{
		writeFile("# comment\na:b\n");

		TestConfigFile cf;
		DGUNIT_ASSERT_EQUAL(true, cf.load());
		DGUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

	void loading_inline_comment()
	{
		writeFile("a:b #comment\n");

		TestConfigFile cf;
		DGUNIT_ASSERT_EQUAL(true, cf.load());
		DGUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

	void loading_single_quoted_string()
	{
		writeFile("a: '#\"b\" ' \n");

		TestConfigFile cf;
		DGUNIT_ASSERT(cf.load());
		DGUNIT_ASSERT_EQUAL(std::string("#\"b\" "), cf.getValue("a"));
	}

	void loading_double_quoted_string()
	{
		writeFile("a: \"#'b' \" \n");

		TestConfigFile cf;
		DGUNIT_ASSERT_EQUAL(true, cf.load());
		DGUNIT_ASSERT_EQUAL(std::string("#'b' "), cf.getValue("a"));
	}

	void loading_error_no_key()
	{
		writeFile(":foo");

		TestConfigFile cf;
		DGUNIT_ASSERT_EQUAL(false, cf.load());
	}

	void loading_error_no_value()
	{
		writeFile("key");

		TestConfigFile cf;
		DGUNIT_ASSERT_EQUAL(false, cf.load());
	}

	void loading_error_string_not_terminated_single()
	{
		writeFile("a:'b\n");

		TestConfigFile cf;
		DGUNIT_ASSERT_EQUAL(false, cf.load());
	}

	void loading_error_string_not_terminated_double()
	{
		writeFile("a:\"b\n");

		TestConfigFile cf;
		DGUNIT_ASSERT_EQUAL(false, cf.load());
	}

	void empty_value()
	{
		writeFile("a:\n");

		TestConfigFile cf;
		DGUNIT_ASSERT_EQUAL(true, cf.load());
		DGUNIT_ASSERT_EQUAL(std::string(""), cf.getValue("a"));
	}
};

// Registers the fixture into the 'registry'
static test_configtest test;

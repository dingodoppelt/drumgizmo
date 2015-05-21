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
#include <cppunit/extensions/HelperMacros.h>

#include <unistd.h>
#include <stdio.h>

#include "../src/configfile.h"

class TestConfigFile : public ConfigFile {
public:
  TestConfigFile() : ConfigFile("") {}

protected:
  // Overload the built-in open method to use local file instead of homedir.
  virtual bool open(std::string mode) override
  {
    fp = fopen("test.conf", mode.c_str());
    return fp != nullptr;
  }
};

class test_configtest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(test_configtest);
	CPPUNIT_TEST(loading_no_newline);
	CPPUNIT_TEST(loading_equal_sign);
	CPPUNIT_TEST(loading_newline);
	CPPUNIT_TEST(loading_padding_space);
	CPPUNIT_TEST(loading_padding_space_newline);
	CPPUNIT_TEST(loading_padding_tab);
	CPPUNIT_TEST(loading_padding_tab_newline);
	CPPUNIT_TEST(loading_comment);
	CPPUNIT_TEST(loading_inline_comment);
	CPPUNIT_TEST(loading_single_quoted_string);
	CPPUNIT_TEST(loading_double_quoted_string);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp()
  {
  }

	void tearDown()
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
    cf.load();
    CPPUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

  void loading_equal_sign()
  {
    writeFile(" a =\tb\t\n");

    TestConfigFile cf;
    cf.load();
    CPPUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

  void loading_newline()
  {
    writeFile("a:b\n");

    TestConfigFile cf;
    cf.load();
    CPPUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

  void loading_padding_space()
  {
    writeFile(" a : b ");

    TestConfigFile cf;
    cf.load();
    CPPUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

  void loading_padding_tab()
  {
    writeFile("\ta\t:\tb\t");

    TestConfigFile cf;
    cf.load();
    CPPUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

  void loading_padding_space_newline()
  {
    writeFile(" a : b \n");

    TestConfigFile cf;
    cf.load();
    CPPUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

  void loading_padding_tab_newline()
  {
    writeFile("\ta\t:\tb\t\n");

    TestConfigFile cf;
    cf.load();
    CPPUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

  void loading_comment()
  {
    writeFile("# comment\na:b\n");

    TestConfigFile cf;
    cf.load();
    CPPUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

  void loading_inline_comment()
  {
    writeFile("a:b #comment\n");

    TestConfigFile cf;
    cf.load();
    CPPUNIT_ASSERT_EQUAL(std::string("b"), cf.getValue("a"));
	}

  void loading_single_quoted_string()
  {
    writeFile("a: '#\"b\" ' \n");

    TestConfigFile cf;
    cf.load();
    CPPUNIT_ASSERT_EQUAL(std::string("#\"b\" "), cf.getValue("a"));
	}

  void loading_double_quoted_string()
  {
    writeFile("a: \"#'b' \" \n");

    TestConfigFile cf;
    cf.load();
    CPPUNIT_ASSERT_EQUAL(std::string("#'b' "), cf.getValue("a"));
	}
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(test_configtest);



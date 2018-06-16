/* -*- Mode: c++ -*- */
/***************************************************************************
 *            dgunit.h
 *
 *  Sat Jun 16 15:44:06 CEST 2018
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
#pragma once

#include <cstddef>
#include <iostream>
#include <list>
#include <functional>
#include <string>
#include <sstream>
#include <fstream>

class DGUnit
{
public:
	DGUnit(const std::string& test_name, const std::string& test_suite = "all")
		: test_name(test_name)
		, test_suite(test_suite)
	{
		suites.emplace_back(this);
	}

	//! Overload to prepare stuff for each of the tests.
	virtual void setup() {}

	//! Overload to tear down stuff for each of the tests.
	virtual void teardown() {}

	struct test_result
	{
		const char* func;
		const char* file;
		std::size_t line;
		std::string msg;
		int id;
	};

	//! Run test
	//! \param test_suite the name of a test suite or null for all.
	//! \param test_name the name of a test name inside a test suite. Only valid
	//!  if test_suite is non-null. nullptr for all tests.
	static int runTests(std::ofstream& out,
	                    const std::string& test_suite = "all",
	                    const std::string& test_name = "all")
	{
		std::size_t test_num{0};
		std::size_t failed{0};

		std::list<test_result> failed_tests;
		std::list<test_result> successful_tests;

		std::cout << "Run tests\n";
		for(auto suite : suites)
		{
			if(test_suite != "all" && suite->test_suite != test_suite)
			{
				continue;
			}

			if(test_name != "all" && suite->test_name != test_name)
			{
				continue;
			}

			std::cout << "Test: " << suite->test_suite << "::"
			          << suite->test_name << std::endl;

			for(auto test : suite->tests)
			{
				++test_num;
				try
				{
					suite->setup();
					test.first();
					suite->teardown();
				}
				catch(test_result& result)
				{
					std::cout << "F";
					fflush(stdout);
					result.id = test_num;
					result.func = test.second;
					failed_tests.push_back(result);
					continue;
				}
				catch(...)
				{
					break; // Uncaught exception. Do not proceed with this test.
				}
				std::cout << ".";
				fflush(stdout);
				test_result result{test.second};
				result.id = test_num;
				successful_tests.emplace_back(result);
			}
		}

		out << "<?xml version=\"1.0\" encoding='ISO-8859-1' standalone='yes' ?>\n";
		out << "<TestRun>\n";
		out << "	<FailedTests>\n";
		for(auto test : failed_tests)
		{
			out << "		<FailedTest id=\"" << test.id << "\">\n";
			out << "			<Name>" << test.func << "</Name>\n";
			out << "			<FailureType>Assertion</FailureType>\n";
			out << "			<Location>\n";
			out << "				<File>" << test.file << "</File>\n";
			out << "				<Line>" << test.line << "</Line>\n";
			out << "			</Location>\n";
			out << "			<Message>" << test.msg << "</Message>\n";
			out << "		</FailedTest>\n";
		}
		out << "	<FailedTests>\n";
		out << "	<SuccessfulTests>\n";
		for(auto test : successful_tests)
		{
			out << "		<Test id=\"" << test.id << "\">\n";
			out << "			<Name>" << test.func << "</Name>\n";
			out << "		</Test>\n";

		}
		out << "	</SuccessfulTests>\n";
		out << "	<Statistics>\n";
		out << "		<Tests>" << (successful_tests.size() + failed_tests.size()) << "</Tests>\n";
		out << "		<FailuresTotal>" << failed_tests.size() << "</FailuresTotal>\n";
		out << "		<Errors>0</Errors>\n";
		out << "		<Failures>" << failed_tests.size() << "</Failures>\n";
		out << "	</Statistics>\n";
		out << "</TestRun>\n";

		return failed == 0 ? 0 : 1;
	}

protected:
	template<typename O, typename F>
	void registerTest(O* obj, const F& fn, const char* name)
	{
		tests.emplace_back(std::make_pair(std::bind(fn, obj), name));
	}
	#define DGUNIT_TEST(func)	  \
		registerTest(this, &func, #func)

	void assert(bool value, const char* expr,
	           const char* file, std::size_t line)
	{
		if(!value)
		{
			std::stringstream ss;
			ss << "assertion failed\n"
				"- Expression: " << expr << "\n";
			throw test_result{"", file, line, ss.str()};
		}
	}
	//! Convenience macro to pass along filename and linenumber
	#define DGUNIT_ASSERT(value)	  \
		assert(value, #value, __FILE__, __LINE__)

	template<typename T>
	void assert_equal(T expected, T value,
	                  const char* file, std::size_t line)
	{
		if(expected != value)
		{
			std::stringstream ss;
			ss << "equality assertion failed\n"
				"- Expected: " << expected << "\n"
				"- Actual  : " << value << "\n";
			throw test_result{"", file, line, ss.str()};
		}
	}
	//! Convenience macro to pass along filename and linenumber
	#define DGUNIT_ASSERT_EQUAL(expected, value) \
		assert_equal(expected, value, __FILE__, __LINE__)

private:
	static std::list<class DGUnit*> suites;
	std::list<std::pair<std::function<void()>, const char*>> tests;
	const std::string test_name;
	const std::string test_suite;
};

#ifdef DGUNIT_MAIN
std::list<class DGUnit*> DGUnit::suites;
#endif

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set et sw=2 ts=2: */
/***************************************************************************
 *            test.h
 *
 *  Wed Dec 16 12:33:19 CET 2009
 *  Copyright 2009 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of Pracro.
 *
 *  Pracro is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Pracro is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Pracro; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#ifndef __PRACRO_TEST_H__
#define __PRACRO_TEST_H__

#include <stdio.h>

#define TEST_REPORT {                                                 \
    fprintf(stderr, "\nTest report:\n%d tests\n%d test failed.\n",    \
            TEST_num_tests, TEST_num_fails);                          \
  }

#define TEST_BEGIN           \
  int main() {               \
  int TEST_num_fails = 0;    \
  int TEST_num_tests = 0;    \
  {}

#define TEST_END {                \
    TEST_REPORT;                  \
    return TEST_num_fails != 0;   \
  } }

#define TEST_OK(m) {                  \
    fprintf(stderr, "  OK: "m"\n");   \
  }

#define TEST_FAIL(m) {                                    \
    fprintf(stderr, "  FAIL: "m"\t\t\t<------------\n");  \
    TEST_num_fails++;                                     \
  }

#define TEST_FATAL(m) {                                   \
  fprintf(stderr, "FATAL: %s\t\t\t<============\n", m);   \
  TEST_num_fails++;                                       \
  { TEST_END; }

#define TEST_MSG(fmt...) {                      \
    fprintf(stderr, "\n");                      \
    fprintf(stderr, fmt);                       \
    fprintf(stderr, " (line %d)\n", __LINE__);  \
  }

#define TEST_BASE(fmt...) {      \
    TEST_num_tests++;            \
    TEST_MSG(fmt);               \
  }

#define TEST_TRUE(x, fmt...) {              \
    TEST_BASE(fmt);                         \
    if(x) { TEST_OK(#x" is true.") }        \
    else { TEST_FAIL(#x" is not true.") }   \
  }

#define TEST_FALSE(x, fmt...) {             \
    TEST_BASE(fmt);                         \
    if(!x) { TEST_OK(#x" is false.") }      \
    else { TEST_FAIL(#x" is not false.") }  \
  }

#define TEST_EQUAL(x, y, fmt...) {                     \
    TEST_BASE(fmt);                                    \
    if(x == y) { TEST_OK(#x" and "#y" are equal.") }   \
    else { TEST_FAIL(#x" and "#y" are not equal.") }   \
  }

#define TEST_NOTEQUAL(x, y, fmt...) {                      \
    TEST_BASE(fmt);                                        \
    if(x != y) { TEST_OK(#x" and "#y" are not equal.") }   \
    else { TEST_FAIL(#x" and "#y" are equal.") }           \
  }

#define TEST_GREATER_THAN(x, y, fmt...) {                 \
    TEST_BASE(fmt);                                       \
    if(x > y) { TEST_OK(#x" are greater than "#y".") }    \
    else { TEST_FAIL(#x" are not greater than "#y".") }   \
  }

#define TEST_LESS_THAN(x, y, fmt...) {                      \
    TEST_BASE(fmt);                                         \
    if(x < y) { TEST_OK(#x" are less than "#y".") }         \
    else { TEST_FAIL(#x" are not less than "#y".") }        \
  }

#define TEST_EQUAL_STR(x, y, fmt...) {                       \
    TEST_BASE(fmt);                                          \
    std::string s1 = x;                                      \
    std::string s2 = y;                                      \
    fprintf(stderr, "Comparing: \"%s\" == \"%s\"\n",         \
            s1.c_str(), s2.c_str());                         \
    if(s1 == s2) {                                           \
      TEST_OK(#x" and "#y" are equal.");                     \
    } else {                                                 \
      TEST_FAIL(#x" and "#y" are not equal.");               \
    }                                                        \
  }

#define TEST_NOTEQUAL_STR(x, y, fmt...) {                    \
    TEST_BASE(fmt);                                          \
    std::string s1 = x;                                      \
    std::string s2 = y;                                      \
    fprintf(stderr, "Comparing: \"%s\" != \"%s\"\n",         \
            s1.c_str(), s2.c_str());                         \
    if(s1 != s2) {                                           \
      TEST_OK(#x" and "#y" not are equal.");                 \
    } else {                                                 \
      TEST_FAIL(#x" and "#y" are equal.");                   \
    }                                                        \
  }

#define TEST_EQUAL_INT(x, y, fmt...) {                        \
    TEST_BASE(fmt);                                           \
    int i1 = x;                                               \
    int i2 = y;                                               \
    fprintf(stderr, "Comparing: \"%d\" == \"%d\"\n", i1, i2); \
    if(i1 == i2) {                                            \
      TEST_OK(#x" and "#y" are equal.");                      \
    } else {                                                  \
      TEST_FAIL(#x" and "#y" are not equal.");                \
    }                                                         \
  }

#define TEST_NOTEQUAL_INT(x, y, fmt...) {                     \
    TEST_BASE(fmt);                                           \
    int i1 = x;                                               \
    int i2 = y;                                               \
    fprintf(stderr, "Comparing: \"%d\" != \"%d\"\n", i1, i2); \
    if(i1 != i2) {                                            \
      TEST_OK(#x" and "#y" are not equal.");                  \
    } else {                                                  \
      TEST_FAIL(#x" and "#y" are equal.");                    \
    }                                                         \
  }

#define TEST_EQUAL_FLOAT(x, y, fmt...) {                      \
    TEST_BASE(fmt);                                           \
    double d1 = x;                                            \
    double d2 = y;                                            \
    fprintf(stderr, "Comparing: \"%f\" == \"%f\"\n", d1, d2); \
    if(d1 == d2) {                                            \
      TEST_OK(#x" and "#y" are equal.");                      \
    } else {                                                  \
      TEST_FAIL(#x" and "#y" are not equal.");                \
    }                                                         \
  }

#define TEST_NOTEQUAL_FLOAT(x, y, fmt...) {                   \
    TEST_BASE(fmt);                                           \
    double d1 = x;                                            \
    double d2 = y;                                            \
    fprintf(stderr, "Comparing: \"%f\" != \"%f\"\n", d1, d2); \
    if(d1 != d2) {                                            \
      TEST_OK(#x" and "#y" are not equal.");                  \
    } else {                                                  \
      TEST_FAIL(#x" and "#y" are equal.");                    \
    }                                                         \
  }

#define TEST_GREATER_THAN_INT(x, y, fmt...) {                  \
    TEST_BASE(fmt);                                            \
    int i1 = x;                                                \
    int i2 = y;                                                \
    fprintf(stderr, "Comparing: \"%d\" > \"%d\"\n", i1, i2);   \
    if(i1 > i2) {                                              \
      TEST_OK(#x" are greater than "#y".");                    \
    } else {                                                   \
      TEST_FAIL(#x" are not greater than "#y".");              \
    }                                                          \
  }

#define TEST_LESS_THAN_INT(x, y, fmt...) {                         \
    TEST_BASE(fmt);                                                \
    int i1 = x;                                                    \
    int i2 = y;                                                    \
    fprintf(stderr, "Comparing: \"%d\" < \"%d\"\n", i1, i2);       \
    if(i1 < i2) {                                                  \
      TEST_OK(#x" are less than "#y".");                           \
    } else {                                                       \
      TEST_FAIL(#x" are not less than "#y".");                     \
    }                                                              \
  }

#define TEST_GREATER_THAN_FLOAT(x, y, fmt...) {                 \
    TEST_BASE(fmt);                                             \
    double d1 = x;                                              \
    double d2 = y;                                              \
    fprintf(stderr, "Comparing: \"%f\" > \"%f\"\n", d1, d2);    \
    if(d1 > d2) {                                               \
      TEST_OK(#x" are greater than "#y".");                     \
    } else {                                                    \
      TEST_FAIL(#x" are not greater than "#y".");               \
    }                                                           \
  }

#define TEST_LESS_THAN_FLOAT(x, y, fmt...) {                       \
    TEST_BASE(fmt);                                                \
    double d1 = x;                                                 \
    double d2 = y;                                                 \
    fprintf(stderr, "Comparing: \"%f\" < \"%f\"\n", d1, d2);       \
    if(d1 < d2) {                                                  \
      TEST_OK(#x" are less than "#y".");                           \
    } else {                                                       \
      TEST_FAIL(#x" are not less than "#y".");                     \
    }                                                              \
  }

#define TEST_EXCEPTION(x, y, fmt...) {                        \
    TEST_BASE(fmt);                                           \
    try {                                                     \
      x;                                                      \
      TEST_FAIL("Exception "#y" was not trown.");             \
    } catch( y &e ) {                                         \
      TEST_OK("Exception "#y" was thrown as expected.");      \
    }                                                         \
  }

#define TEST_NOTEXCEPTION(x, y, fmt...) {                     \
    TEST_BASE(fmt);                                           \
    try {                                                     \
      x;                                                      \
      TEST_OK("Exception "#y" was not trown as expected");    \
    } catch( y &e ) {                                         \
      TEST_FAIL("Exception "#y" was thrown.");                \
    }                                                         \
  }

#define TEST_NOEXCEPTION(x, fmt...) {                         \
    TEST_BASE(fmt);                                           \
    try {                                                     \
      x;                                                      \
      TEST_OK("Exception was not trown as expected");         \
    } catch( ... ) {                                          \
      TEST_FAIL("Exception was thrown.");                     \
    }                                                         \
  }

#endif/*__PRACRO_TEST_H__*/

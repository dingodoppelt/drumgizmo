/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            nolocale.h
 *
 *  Fri Feb 13 12:48:10 CET 2015
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
#ifndef __DRUMGIZMO_NOLOCALE_H__
#define __DRUMGIZMO_NOLOCALE_H__

#include <locale.h>
#include <stdarg.h>

static inline double atof_nol(const char *nptr)
{
	double res;

#ifdef WIN32

  _locale_t l = _create_locale(LC_NUMERIC, "C");

  res = _atof_l(nptr, l);

  _free_locale(l);

#else/*WIN32*/

	locale_t new_locale, prev_locale;

	new_locale = newlocale(LC_NUMERIC_MASK, "C", NULL);
	prev_locale = uselocale(new_locale);

	res = atof(nptr);

	uselocale(prev_locale);
	freelocale(new_locale);

#endif/*WIN32*/

	return res;
}

static inline int sprintf_nol(char *str, const char *format, ...)
{
  int ret;

#ifdef WIN32

  _locale_t l = _create_locale(LC_NUMERIC, "C");

  va_list vl;
  va_start(vl, format);
  ret = _vsprintf_l(str, format, vl, l);
  va_end(vl);

  _free_locale(l);

#else/*WIN32*/

	locale_t new_locale, prev_locale;

	new_locale = newlocale(LC_NUMERIC_MASK, "C", NULL);
	prev_locale = uselocale(new_locale);

  va_list vl;
  va_start(vl, format);
  ret = vsprintf(str, format, vl);
  va_end(vl);

	uselocale(prev_locale);
	freelocale(new_locale);

#endif/*WIN32*/

  return ret;
}

static inline int snprintf_nol(char *str, size_t size, const char *format, ...)
{
  int ret;

#ifdef WIN32

  _locale_t l = _create_locale(LC_NUMERIC, "C");

  va_list vl;
  va_start(vl, format);
  ret = _vsnprintf_l(str, size, format, vl, l);
  va_end(vl);

  _free_locale(l);

#else/*WIN32*/

	locale_t new_locale, prev_locale;

	new_locale = newlocale(LC_NUMERIC_MASK, "C", NULL);
	prev_locale = uselocale(new_locale);

  va_list vl;
  va_start(vl, format);
  ret = vsnprintf(str, size, format, vl);
  va_end(vl);

	uselocale(prev_locale);
	freelocale(new_locale);

#endif/*WIN32*/

  return ret;
}

#endif/*__DRUMGIZMO_NOLOCALE_H__*/

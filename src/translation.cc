/* -*- Mode: c++ -*- */
/***************************************************************************
 *            translation.cc
 *
 *  Sun Sep  8 14:13:22 CEST 2019
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
#include "translation.h"

#ifdef WITH_NLS

#include <locale.h>
#include <vector>
#include <mutex>
#include <string>
#include <atomic>
#include <algorithm>
#include <utility>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace
{
using Text = std::pair<std::uint64_t, std::string>;
using Texts = std::vector<Text>;

struct
{
	std::mutex mutex;
	int refcnt{0};
	Texts texts;
} singleton;


bool comparator(const Text& a, const Text& b)
{
	return a.first < b.first;
}
}

Translation::Translation()
{
	std::lock_guard<std::mutex>(singleton.mutex);
	++singleton.refcnt;
}

Translation::~Translation()
{
	std::lock_guard<std::mutex>(singleton.mutex);

	--singleton.refcnt;

	if(singleton.refcnt == 0)
	{
		singleton.texts.clear();
	}
}

bool Translation::load(const char* catalog, std::size_t size)
{
	Texts texts;

	// https://www.gnu.org/software/gettext/manual/html_node/MO-Files.html

	//         byte
	//              +------------------------------------------+
	//           0  | magic number = 0x950412de                |
	//              |                                          |
	//           4  | file format revision = 0                 |
	//              |                                          |
	//           8  | number of strings                        |  == N
	//              |                                          |
	//          12  | offset of table with original strings    |  == O
	//              |                                          |
	//          16  | offset of table with translation strings |  == T
	//              |                                          |
	//          20  | size of hashing table                    |  == S
	//              |                                          |
	//          24  | offset of hashing table                  |  == H
	//              |                                          |
	//              .                                          .
	//              .    (possibly more entries later)         .
	//              .                                          .
	//              |                                          |
	//           O  | length & offset 0th string  ----------------.
	//       O + 8  | length & offset 1st string  ------------------.
	//               ...                                    ...   | |
	// O + ((N-1)*8)| length & offset (N-1)th string           |  | |
	//              |                                          |  | |
	//           T  | length & offset 0th translation  ---------------.
	//       T + 8  | length & offset 1st translation  -----------------.
	//               ...                                    ...   | | | |
	// T + ((N-1)*8)| length & offset (N-1)th translation      |  | | | |
	//              |                                          |  | | | |
	//           H  | start hash table                         |  | | | |
	//               ...                                    ...   | | | |
	//   H + S * 4  | end hash table                           |  | | | |
	//              |                                          |  | | | |
	//              | NUL terminated 0th string  <----------------' | | |
	//              |                                          |    | | |
	//              | NUL terminated 1st string  <------------------' | |
	//              |                                          |      | |
	//               ...                                    ...       | |
	//              |                                          |      | |
	//              | NUL terminated 0th translation  <---------------' |
	//              |                                          |        |
	//              | NUL terminated 1st translation  <-----------------'
	//              |                                          |
	//               ...                                    ...
	//              |                                          |
	//              +------------------------------------------+
	constexpr std::uint32_t magic_number_le = 0x950412de;
	constexpr std::uint32_t magic_number_be = 0xde120495;

	const char* ptr = catalog;

	// Verify magic number
	std::uint32_t magic_number = *reinterpret_cast<const std::uint32_t*>(ptr);
	if(magic_number != magic_number_le && magic_number != magic_number_be)
	{
		// Error - bad magic number
		return false;
	}
	ptr += sizeof(magic_number);

	// Verify file format revision
	std::uint32_t file_format_revision =
		*reinterpret_cast<const std::uint32_t*>(ptr);
	if(file_format_revision != 0)
	{
		// Error - bad file format revision
		return false;
	}
	ptr += sizeof(file_format_revision);

	// Read number of string in the catalog
	std::uint32_t number_of_strings =
		*reinterpret_cast<const std::uint32_t*>(ptr);
	ptr += sizeof(number_of_strings);

	// Read orig_table_offset
	std::uint32_t orig_table_offset =
		*reinterpret_cast<const std::uint32_t*>(ptr);
	ptr += sizeof(orig_table_offset);

	// Read transl_table_offset
	std::uint32_t transl_table_offset =
		*reinterpret_cast<const std::uint32_t*>(ptr);
	ptr += sizeof(transl_table_offset);

	// Read hash_table_size
	std::uint32_t hash_table_size =
		*reinterpret_cast<const std::uint32_t*>(ptr);
	ptr += sizeof(hash_table_size);

	// Read hash_table_offset
	std::uint32_t hash_table_offset =
		*reinterpret_cast<const std::uint32_t*>(ptr);
	ptr += sizeof(hash_table_offset);


	// Read original/translated string tables:
	const char* orig_ptr = catalog + orig_table_offset;
	const char* transl_ptr = catalog + transl_table_offset;
	for(std::uint32_t string_index = 0;
	    string_index < number_of_strings; ++string_index)
	{
		// Read string_length
		std::uint32_t orig_string_length =
			*reinterpret_cast<const std::uint32_t*>(orig_ptr);
		orig_ptr += sizeof(orig_string_length);

		// Read string_offset
		std::uint32_t orig_string_offset =
			*reinterpret_cast<const std::uint32_t*>(orig_ptr);
		orig_ptr += sizeof(orig_string_offset);

		std::string orig;
		orig.append(catalog + orig_string_offset, orig_string_length);

		// Read string_length
		std::uint32_t transl_string_length =
			*reinterpret_cast<const std::uint32_t*>(transl_ptr);
		transl_ptr += sizeof(transl_string_length);

		// Read string_offset
		std::uint32_t transl_string_offset =
			*reinterpret_cast<const std::uint32_t*>(transl_ptr);
		transl_ptr += sizeof(transl_string_offset);

		std::string transl;
		transl.append(catalog + transl_string_offset, transl_string_length);

		texts.push_back(make_pair(const_hash(orig.data()), transl));
	}

	std::sort(texts.begin(), texts.end(), comparator);

	{
		std::lock_guard<std::mutex>(singleton.mutex);
		std::swap(singleton.texts, texts);
	}

	return true;
}

const char* Translation::gettext(std::uint64_t msgid, const char* original)
{
	std::lock_guard<std::mutex>(singleton.mutex);
	if(singleton.refcnt == 0)
	{
		return original;
	}

	auto it = std::lower_bound(singleton.texts.begin(),
	                           singleton.texts.end(),
	                           make_pair(msgid, std::string()), comparator);
	if(it == singleton.texts.end() ||
	   it->first != msgid)
	{
		return original;
	}

	return it->second.data();
}

std::string Translation::getISO639LanguageName()
{
	std::string lang;

#ifdef _WIN32
	LCID lcid = GetUserDefaultLCID();
	char name[LOCALE_NAME_MAX_LENGTH];
	GetLocaleInfo(lcid, LOCALE_SISO639LANGNAME, name, LOCALE_NAME_MAX_LENGTH);
	lang = name;
#else
	try
	{
		auto langstr = setlocale(LC_ALL, "");
		if(langstr != nullptr)
		{
			lang = langstr;
		}
	}
	catch(...)
	{
		// Bad locale: don't load anything - use default
		printf("Bad locale: don't load anything - use default\n");
		return "";
	}

	if(lang == "C")
	{
		printf("Don't load anything - use default\n");
		return ""; // Don't load anything - use default
	}

	auto _ = lang.find('_');
	lang = lang.substr(0, _);
#endif

	return lang;
}

#endif // WITH_NLS

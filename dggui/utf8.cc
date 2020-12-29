/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            utf8.cc
 *
 *  Tue Feb 27 19:18:23 CET 2007
 *  Copyright  2006 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
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
#include "utf8.h"

UTF8::UTF8()
{
	// Encode Map
	map_encode["\x80"] = "\xc2\x80";
	map_encode["\x81"] = "\xc2\x81";
	map_encode["\x82"] = "\xc2\x82";
	map_encode["\x83"] = "\xc2\x83";
	map_encode["\x84"] = "\xc2\x84";
	map_encode["\x85"] = "\xc2\x85";
	map_encode["\x86"] = "\xc2\x86";
	map_encode["\x87"] = "\xc2\x87";
	map_encode["\x88"] = "\xc2\x88";
	map_encode["\x89"] = "\xc2\x89";
	map_encode["\x8a"] = "\xc2\x8a";
	map_encode["\x8b"] = "\xc2\x8b";
	map_encode["\x8c"] = "\xc2\x8c";
	map_encode["\x8d"] = "\xc2\x8d";
	map_encode["\x8e"] = "\xc2\x8e";
	map_encode["\x8f"] = "\xc2\x8f";
	map_encode["\x90"] = "\xc2\x90";
	map_encode["\x91"] = "\xc2\x91";
	map_encode["\x92"] = "\xc2\x92";
	map_encode["\x93"] = "\xc2\x93";
	map_encode["\x94"] = "\xc2\x94";
	map_encode["\x95"] = "\xc2\x95";
	map_encode["\x96"] = "\xc2\x96";
	map_encode["\x97"] = "\xc2\x97";
	map_encode["\x98"] = "\xc2\x98";
	map_encode["\x99"] = "\xc2\x99";
	map_encode["\x9a"] = "\xc2\x9a";
	map_encode["\x9b"] = "\xc2\x9b";
	map_encode["\x9c"] = "\xc2\x9c";
	map_encode["\x9d"] = "\xc2\x9d";
	map_encode["\x9e"] = "\xc2\x9e";
	map_encode["\x9f"] = "\xc2\x9f";
	map_encode["\xa0"] = "\xc2\xa0";
	map_encode["\xa1"] = "\xc2\xa1";
	map_encode["\xa2"] = "\xc2\xa2";
	map_encode["\xa3"] = "\xc2\xa3";
	map_encode["\xa4"] = "\xc2\xa4";
	map_encode["\xa5"] = "\xc2\xa5";
	map_encode["\xa6"] = "\xc2\xa6";
	map_encode["\xa7"] = "\xc2\xa7";
	map_encode["\xa8"] = "\xc2\xa8";
	map_encode["\xa9"] = "\xc2\xa9";
	map_encode["\xaa"] = "\xc2\xaa";
	map_encode["\xab"] = "\xc2\xab";
	map_encode["\xac"] = "\xc2\xac";
	map_encode["\xad"] = "\xc2\xad";
	map_encode["\xae"] = "\xc2\xae";
	map_encode["\xaf"] = "\xc2\xaf";
	map_encode["\xb0"] = "\xc2\xb0";
	map_encode["\xb1"] = "\xc2\xb1";
	map_encode["\xb2"] = "\xc2\xb2";
	map_encode["\xb3"] = "\xc2\xb3";
	map_encode["\xb4"] = "\xc2\xb4";
	map_encode["\xb5"] = "\xc2\xb5";
	map_encode["\xb6"] = "\xc2\xb6";
	map_encode["\xb7"] = "\xc2\xb7";
	map_encode["\xb8"] = "\xc2\xb8";
	map_encode["\xb9"] = "\xc2\xb9";
	map_encode["\xba"] = "\xc2\xba";
	map_encode["\xbb"] = "\xc2\xbb";
	map_encode["\xbc"] = "\xc2\xbc";
	map_encode["\xbd"] = "\xc2\xbd";
	map_encode["\xbe"] = "\xc2\xbe";
	map_encode["\xbf"] = "\xc2\xbf";
	map_encode["\xc0"] = "\xc3\x80";
	map_encode["\xc1"] = "\xc3\x81";
	map_encode["\xc2"] = "\xc3\x82";
	map_encode["\xc3"] = "\xc3\x83";
	map_encode["\xc4"] = "\xc3\x84";
	map_encode["\xc5"] = "\xc3\x85";
	map_encode["\xc6"] = "\xc3\x86";
	map_encode["\xc7"] = "\xc3\x87";
	map_encode["\xc8"] = "\xc3\x88";
	map_encode["\xc9"] = "\xc3\x89";
	map_encode["\xca"] = "\xc3\x8a";
	map_encode["\xcb"] = "\xc3\x8b";
	map_encode["\xcc"] = "\xc3\x8c";
	map_encode["\xcd"] = "\xc3\x8d";
	map_encode["\xce"] = "\xc3\x8e";
	map_encode["\xcf"] = "\xc3\x8f";
	map_encode["\xd0"] = "\xc3\x90";
	map_encode["\xd1"] = "\xc3\x91";
	map_encode["\xd2"] = "\xc3\x92";
	map_encode["\xd3"] = "\xc3\x93";
	map_encode["\xd4"] = "\xc3\x94";
	map_encode["\xd5"] = "\xc3\x95";
	map_encode["\xd6"] = "\xc3\x96";
	map_encode["\xd7"] = "\xc3\x97";
	map_encode["\xd8"] = "\xc3\x98";
	map_encode["\xd9"] = "\xc3\x99";
	map_encode["\xda"] = "\xc3\x9a";
	map_encode["\xdb"] = "\xc3\x9b";
	map_encode["\xdc"] = "\xc3\x9c";
	map_encode["\xdd"] = "\xc3\x9d";
	map_encode["\xde"] = "\xc3\x9e";
	map_encode["\xdf"] = "\xc3\x9f";
	map_encode["\xe0"] = "\xc3\xa0";
	map_encode["\xe1"] = "\xc3\xa1";
	map_encode["\xe2"] = "\xc3\xa2";
	map_encode["\xe3"] = "\xc3\xa3";
	map_encode["\xe4"] = "\xc3\xa4";
	map_encode["\xe5"] = "\xc3\xa5";
	map_encode["\xe6"] = "\xc3\xa6";
	map_encode["\xe7"] = "\xc3\xa7";
	map_encode["\xe8"] = "\xc3\xa8";
	map_encode["\xe9"] = "\xc3\xa9";
	map_encode["\xea"] = "\xc3\xaa";
	map_encode["\xeb"] = "\xc3\xab";
	map_encode["\xec"] = "\xc3\xac";
	map_encode["\xed"] = "\xc3\xad";
	map_encode["\xee"] = "\xc3\xae";
	map_encode["\xef"] = "\xc3\xaf";
	map_encode["\xf0"] = "\xc3\xb0";
	map_encode["\xf1"] = "\xc3\xb1";
	map_encode["\xf2"] = "\xc3\xb2";
	map_encode["\xf3"] = "\xc3\xb3";
	map_encode["\xf4"] = "\xc3\xb4";
	map_encode["\xf5"] = "\xc3\xb5";
	map_encode["\xf6"] = "\xc3\xb6";
	map_encode["\xf7"] = "\xc3\xb7";
	map_encode["\xf8"] = "\xc3\xb8";
	map_encode["\xf9"] = "\xc3\xb9";
	map_encode["\xfa"] = "\xc3\xba";
	map_encode["\xfb"] = "\xc3\xbb";
	map_encode["\xfc"] = "\xc3\xbc";
	map_encode["\xfd"] = "\xc3\xbd";
	map_encode["\xfe"] = "\xc3\xbe";
	map_encode["\xff"] = "\xc3\xbf";

	// Decode Map
	map_decode["\xc2\x80"] = "\x80";
	map_decode["\xc2\x81"] = "\x81";
	map_decode["\xc2\x82"] = "\x82";
	map_decode["\xc2\x83"] = "\x83";
	map_decode["\xc2\x84"] = "\x84";
	map_decode["\xc2\x85"] = "\x85";
	map_decode["\xc2\x86"] = "\x86";
	map_decode["\xc2\x87"] = "\x87";
	map_decode["\xc2\x88"] = "\x88";
	map_decode["\xc2\x89"] = "\x89";
	map_decode["\xc2\x8a"] = "\x8a";
	map_decode["\xc2\x8b"] = "\x8b";
	map_decode["\xc2\x8c"] = "\x8c";
	map_decode["\xc2\x8d"] = "\x8d";
	map_decode["\xc2\x8e"] = "\x8e";
	map_decode["\xc2\x8f"] = "\x8f";
	map_decode["\xc2\x90"] = "\x90";
	map_decode["\xc2\x91"] = "\x91";
	map_decode["\xc2\x92"] = "\x92";
	map_decode["\xc2\x93"] = "\x93";
	map_decode["\xc2\x94"] = "\x94";
	map_decode["\xc2\x95"] = "\x95";
	map_decode["\xc2\x96"] = "\x96";
	map_decode["\xc2\x97"] = "\x97";
	map_decode["\xc2\x98"] = "\x98";
	map_decode["\xc2\x99"] = "\x99";
	map_decode["\xc2\x9a"] = "\x9a";
	map_decode["\xc2\x9b"] = "\x9b";
	map_decode["\xc2\x9c"] = "\x9c";
	map_decode["\xc2\x9d"] = "\x9d";
	map_decode["\xc2\x9e"] = "\x9e";
	map_decode["\xc2\x9f"] = "\x9f";
	map_decode["\xc2\xa0"] = "\xa0";
	map_decode["\xc2\xa1"] = "\xa1";
	map_decode["\xc2\xa2"] = "\xa2";
	map_decode["\xc2\xa3"] = "\xa3";
	map_decode["\xc2\xa4"] = "\xa4";
	map_decode["\xc2\xa5"] = "\xa5";
	map_decode["\xc2\xa6"] = "\xa6";
	map_decode["\xc2\xa7"] = "\xa7";
	map_decode["\xc2\xa8"] = "\xa8";
	map_decode["\xc2\xa9"] = "\xa9";
	map_decode["\xc2\xaa"] = "\xaa";
	map_decode["\xc2\xab"] = "\xab";
	map_decode["\xc2\xac"] = "\xac";
	map_decode["\xc2\xad"] = "\xad";
	map_decode["\xc2\xae"] = "\xae";
	map_decode["\xc2\xaf"] = "\xaf";
	map_decode["\xc2\xb0"] = "\xb0";
	map_decode["\xc2\xb1"] = "\xb1";
	map_decode["\xc2\xb2"] = "\xb2";
	map_decode["\xc2\xb3"] = "\xb3";
	map_decode["\xc2\xb4"] = "\xb4";
	map_decode["\xc2\xb5"] = "\xb5";
	map_decode["\xc2\xb6"] = "\xb6";
	map_decode["\xc2\xb7"] = "\xb7";
	map_decode["\xc2\xb8"] = "\xb8";
	map_decode["\xc2\xb9"] = "\xb9";
	map_decode["\xc2\xba"] = "\xba";
	map_decode["\xc2\xbb"] = "\xbb";
	map_decode["\xc2\xbc"] = "\xbc";
	map_decode["\xc2\xbd"] = "\xbd";
	map_decode["\xc2\xbe"] = "\xbe";
	map_decode["\xc2\xbf"] = "\xbf";
	map_decode["\xc3\x80"] = "\xc0";
	map_decode["\xc3\x81"] = "\xc1";
	map_decode["\xc3\x82"] = "\xc2";
	map_decode["\xc3\x83"] = "\xc3";
	map_decode["\xc3\x84"] = "\xc4";
	map_decode["\xc3\x85"] = "\xc5";
	map_decode["\xc3\x86"] = "\xc6";
	map_decode["\xc3\x87"] = "\xc7";
	map_decode["\xc3\x88"] = "\xc8";
	map_decode["\xc3\x89"] = "\xc9";
	map_decode["\xc3\x8a"] = "\xca";
	map_decode["\xc3\x8b"] = "\xcb";
	map_decode["\xc3\x8c"] = "\xcc";
	map_decode["\xc3\x8d"] = "\xcd";
	map_decode["\xc3\x8e"] = "\xce";
	map_decode["\xc3\x8f"] = "\xcf";
	map_decode["\xc3\x90"] = "\xd0";
	map_decode["\xc3\x91"] = "\xd1";
	map_decode["\xc3\x92"] = "\xd2";
	map_decode["\xc3\x93"] = "\xd3";
	map_decode["\xc3\x94"] = "\xd4";
	map_decode["\xc3\x95"] = "\xd5";
	map_decode["\xc3\x96"] = "\xd6";
	map_decode["\xc3\x97"] = "\xd7";
	map_decode["\xc3\x98"] = "\xd8";
	map_decode["\xc3\x99"] = "\xd9";
	map_decode["\xc3\x9a"] = "\xda";
	map_decode["\xc3\x9b"] = "\xdb";
	map_decode["\xc3\x9c"] = "\xdc";
	map_decode["\xc3\x9d"] = "\xdd";
	map_decode["\xc3\x9e"] = "\xde";
	map_decode["\xc3\x9f"] = "\xdf";
	map_decode["\xc3\xa0"] = "\xe0";
	map_decode["\xc3\xa1"] = "\xe1";
	map_decode["\xc3\xa2"] = "\xe2";
	map_decode["\xc3\xa3"] = "\xe3";
	map_decode["\xc3\xa4"] = "\xe4";
	map_decode["\xc3\xa5"] = "\xe5";
	map_decode["\xc3\xa6"] = "\xe6";
	map_decode["\xc3\xa7"] = "\xe7";
	map_decode["\xc3\xa8"] = "\xe8";
	map_decode["\xc3\xa9"] = "\xe9";
	map_decode["\xc3\xaa"] = "\xea";
	map_decode["\xc3\xab"] = "\xeb";
	map_decode["\xc3\xac"] = "\xec";
	map_decode["\xc3\xad"] = "\xed";
	map_decode["\xc3\xae"] = "\xee";
	map_decode["\xc3\xaf"] = "\xef";
	map_decode["\xc3\xb0"] = "\xf0";
	map_decode["\xc3\xb1"] = "\xf1";
	map_decode["\xc3\xb2"] = "\xf2";
	map_decode["\xc3\xb3"] = "\xf3";
	map_decode["\xc3\xb4"] = "\xf4";
	map_decode["\xc3\xb5"] = "\xf5";
	map_decode["\xc3\xb6"] = "\xf6";
	map_decode["\xc3\xb7"] = "\xf7";
	map_decode["\xc3\xb8"] = "\xf8";
	map_decode["\xc3\xb9"] = "\xf9";
	map_decode["\xc3\xba"] = "\xfa";
	map_decode["\xc3\xbb"] = "\xfb";
	map_decode["\xc3\xbc"] = "\xfc";
	map_decode["\xc3\xbd"] = "\xfd";
	map_decode["\xc3\xbe"] = "\xfe";
	map_decode["\xc3\xbf"] = "\xff";
	// FIXME: This is just a hack to make Goran Mekic's name work.
	map_decode["\xc4\x87"] = "c";
}

std::string UTF8::fromLatin1(std::string const& s)
{
	std::string ret;

	for(int i = 0; i < (int)s.length(); i++)
	{
		std::string c;

		if((unsigned char)s[i] <= 0x7F)
		{
			c = s.substr(i, 1);
		}
		else
		{
			c = map_encode[s.substr(i, 1)];
		}

		// If c == "", the character wasn't found in the map.
		// Ignore this case for now and just push an empty string in this case.

		ret.append(c);
	}

	return ret;
}

std::string UTF8::toLatin1(std::string const& s)
{
	std::string ret;

	int width = 1;
	for(int i = 0; i < (int)s.length(); i += width)
	{
		if(/*(unsigned char)s[i]>=0x00&&*/ (unsigned char)s[i] <= 0x7F)
		{
			width = 1; // 00-7F -> 1 byte
		}
		if((unsigned char)s[i] >= 0xC2 && (unsigned char)s[i] <= 0xDF)
		{
			width = 2; // C2-DF -> 2 bytes
		}
		if((unsigned char)s[i] >= 0xE0 && (unsigned char)s[i] <= 0xEF)
		{
			width = 3; // E0-EF -> 3 bytes
		}
		if((unsigned char)s[i] >= 0xF0 && (unsigned char)s[i] <= 0xF4)
		{
			width = 4; // F0-F4 -> 4 bytes
		}

		std::string c;
		if(width == 1)
		{
			c = s.substr(i, 1);
		}
		else
		{
			c = map_decode[s.substr(i, width)];
		}

		// If c == "", the character wasn't found in the map.
		// Ignore this case for now and just push an empty string in this case.

		ret.append(c);
	}

	return ret;
}

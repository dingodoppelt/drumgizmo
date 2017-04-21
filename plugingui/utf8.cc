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
#include "utf8.h"

UTF8::UTF8()
{
	// Encode Map
	map_encode["\x80"] = "Â\x80";
	map_encode["\x81"] = "Â\x81";
	map_encode["\x82"] = "Â\x82";
	map_encode["\x83"] = "Â\x83";
	map_encode["\x84"] = "Â\x84";
	map_encode["\x85"] = "Â\x85";
	map_encode["\x86"] = "Â\x86";
	map_encode["\x87"] = "Â\x87";
	map_encode["\x88"] = "Â\x88";
	map_encode["\x89"] = "Â\x89";
	map_encode["\x8a"] = "Â\x8a";
	map_encode["\x8b"] = "Â\x8b";
	map_encode["\x8c"] = "Â\x8c";
	map_encode["\x8d"] = "Â\x8d";
	map_encode["\x8e"] = "Â\x8e";
	map_encode["\x8f"] = "Â\x8f";
	map_encode["\x90"] = "Â\x90";
	map_encode["\x91"] = "Â\x91";
	map_encode["\x92"] = "Â\x92";
	map_encode["\x93"] = "Â\x93";
	map_encode["\x94"] = "Â\x94";
	map_encode["\x95"] = "Â\x95";
	map_encode["\x96"] = "Â\x96";
	map_encode["\x97"] = "Â\x97";
	map_encode["\x98"] = "Â\x98";
	map_encode["\x99"] = "Â\x99";
	map_encode["\x9a"] = "Â\x9a";
	map_encode["\x9b"] = "Â\x9b";
	map_encode["\x9c"] = "Â\x9c";
	map_encode["\x9d"] = "Â\x9d";
	map_encode["\x9e"] = "Â\x9e";
	map_encode["\x9f"] = "Â\x9f";
	map_encode[" "] = "Â ";
	map_encode["¡"] = "Â¡";
	map_encode["¢"] = "Â¢";
	map_encode["£"] = "Â£";
	map_encode["¤"] = "Â¤";
	map_encode["¥"] = "Â¥";
	map_encode["¦"] = "Â¦";
	map_encode["§"] = "Â§";
	map_encode["¨"] = "Â¨";
	map_encode["©"] = "Â©";
	map_encode["ª"] = "Âª";
	map_encode["«"] = "Â«";
	map_encode["¬"] = "Â¬";
	map_encode["­"] = "Â­";
	map_encode["®"] = "Â®";
	map_encode["¯"] = "Â¯";
	map_encode["°"] = "Â°";
	map_encode["±"] = "Â±";
	map_encode["²"] = "Â²";
	map_encode["³"] = "Â³";
	map_encode["´"] = "Â´";
	map_encode["µ"] = "Âµ";
	map_encode["¶"] = "Â¶";
	map_encode["·"] = "Â·";
	map_encode["¸"] = "Â¸";
	map_encode["¹"] = "Â¹";
	map_encode["º"] = "Âº";
	map_encode["»"] = "Â»";
	map_encode["¼"] = "Â¼";
	map_encode["½"] = "Â½";
	map_encode["¾"] = "Â¾";
	map_encode["¿"] = "Â¿";
	map_encode["À"] = "Ã\x80";
	map_encode["Á"] = "Ã\x81";
	map_encode["Â"] = "Ã\x82";
	map_encode["Ã"] = "Ã\x83";
	map_encode["Ä"] = "Ã\x84";
	map_encode["Å"] = "Ã\x85";
	map_encode["Æ"] = "Ã\x86";
	map_encode["Ç"] = "Ã\x87";
	map_encode["È"] = "Ã\x88";
	map_encode["É"] = "Ã\x89";
	map_encode["Ê"] = "Ã\x8a";
	map_encode["Ë"] = "Ã\x8b";
	map_encode["Ì"] = "Ã\x8c";
	map_encode["Í"] = "Ã\x8d";
	map_encode["Î"] = "Ã\x8e";
	map_encode["Ï"] = "Ã\x8f";
	map_encode["Ð"] = "Ã\x90";
	map_encode["Ñ"] = "Ã\x91";
	map_encode["Ò"] = "Ã\x92";
	map_encode["Ó"] = "Ã\x93";
	map_encode["Ô"] = "Ã\x94";
	map_encode["Õ"] = "Ã\x95";
	map_encode["Ö"] = "Ã\x96";
	map_encode["×"] = "Ã\x97";
	map_encode["Ø"] = "Ã\x98";
	map_encode["Ù"] = "Ã\x99";
	map_encode["Ú"] = "Ã\x9a";
	map_encode["Û"] = "Ã\x9b";
	map_encode["Ü"] = "Ã\x9c";
	map_encode["Ý"] = "Ã\x9d";
	map_encode["Þ"] = "Ã\x9e";
	map_encode["ß"] = "Ã\x9f";
	map_encode["à"] = "Ã ";
	map_encode["á"] = "Ã¡";
	map_encode["â"] = "Ã¢";
	map_encode["ã"] = "Ã£";
	map_encode["ä"] = "Ã¤";
	map_encode["å"] = "Ã¥";
	map_encode["æ"] = "Ã¦";
	map_encode["ç"] = "Ã§";
	map_encode["è"] = "Ã¨";
	map_encode["é"] = "Ã©";
	map_encode["ê"] = "Ãª";
	map_encode["ë"] = "Ã«";
	map_encode["ì"] = "Ã¬";
	map_encode["í"] = "Ã­";
	map_encode["î"] = "Ã®";
	map_encode["ï"] = "Ã¯";
	map_encode["ð"] = "Ã°";
	map_encode["ñ"] = "Ã±";
	map_encode["ò"] = "Ã²";
	map_encode["ó"] = "Ã³";
	map_encode["ô"] = "Ã´";
	map_encode["õ"] = "Ãµ";
	map_encode["ö"] = "Ã¶";
	map_encode["÷"] = "Ã·";
	map_encode["ø"] = "Ã¸";
	map_encode["ù"] = "Ã¹";
	map_encode["ú"] = "Ãº";
	map_encode["û"] = "Ã»";
	map_encode["ü"] = "Ã¼";
	map_encode["ý"] = "Ã½";
	map_encode["þ"] = "Ã¾";
	map_encode["ÿ"] = "Ã¿";

	// Decode Map
	map_decode["Â\x80"] = "\x80";
	map_decode["Â\x81"] = "\x81";
	map_decode["Â\x82"] = "\x82";
	map_decode["Â\x83"] = "\x83";
	map_decode["Â\x84"] = "\x84";
	map_decode["Â\x85"] = "\x85";
	map_decode["Â\x86"] = "\x86";
	map_decode["Â\x87"] = "\x87";
	map_decode["Â\x88"] = "\x88";
	map_decode["Â\x89"] = "\x89";
	map_decode["Â\x8a"] = "\x8a";
	map_decode["Â\x8b"] = "\x8b";
	map_decode["Â\x8c"] = "\x8c";
	map_decode["Â\x8d"] = "\x8d";
	map_decode["Â\x8e"] = "\x8e";
	map_decode["Â\x8f"] = "\x8f";
	map_decode["Â\x90"] = "\x90";
	map_decode["Â\x91"] = "\x91";
	map_decode["Â\x92"] = "\x92";
	map_decode["Â\x93"] = "\x93";
	map_decode["Â\x94"] = "\x94";
	map_decode["Â\x95"] = "\x95";
	map_decode["Â\x96"] = "\x96";
	map_decode["Â\x97"] = "\x97";
	map_decode["Â\x98"] = "\x98";
	map_decode["Â\x99"] = "\x99";
	map_decode["Â\x9a"] = "\x9a";
	map_decode["Â\x9b"] = "\x9b";
	map_decode["Â\x9c"] = "\x9c";
	map_decode["Â\x9d"] = "\x9d";
	map_decode["Â\x9e"] = "\x9e";
	map_decode["Â\x9f"] = "\x9f";
	map_decode["Â "] = " ";
	map_decode["Â¡"] = "¡";
	map_decode["Â¢"] = "¢";
	map_decode["Â£"] = "£";
	map_decode["Â¤"] = "¤";
	map_decode["Â¥"] = "¥";
	map_decode["Â¦"] = "¦";
	map_decode["Â§"] = "§";
	map_decode["Â¨"] = "¨";
	map_decode["Â©"] = "©";
	map_decode["Âª"] = "ª";
	map_decode["Â«"] = "«";
	map_decode["Â¬"] = "¬";
	map_decode["Â­"] = "­";
	map_decode["Â®"] = "®";
	map_decode["Â¯"] = "¯";
	map_decode["Â°"] = "°";
	map_decode["Â±"] = "±";
	map_decode["Â²"] = "²";
	map_decode["Â³"] = "³";
	map_decode["Â´"] = "´";
	map_decode["Âµ"] = "µ";
	map_decode["Â¶"] = "¶";
	map_decode["Â·"] = "·";
	map_decode["Â¸"] = "¸";
	map_decode["Â¹"] = "¹";
	map_decode["Âº"] = "º";
	map_decode["Â»"] = "»";
	map_decode["Â¼"] = "¼";
	map_decode["Â½"] = "½";
	map_decode["Â¾"] = "¾";
	map_decode["Â¿"] = "¿";
	map_decode["Ã\x80"] = "À";
	map_decode["Ã\x81"] = "Á";
	map_decode["Ã\x82"] = "Â";
	map_decode["Ã\x83"] = "Ã";
	map_decode["Ã\x84"] = "Ä";
	map_decode["Ã\x85"] = "Å";
	map_decode["Ã\x86"] = "Æ";
	map_decode["Ã\x87"] = "Ç";
	map_decode["Ã\x88"] = "È";
	map_decode["Ã\x89"] = "É";
	map_decode["Ã\x8a"] = "Ê";
	map_decode["Ã\x8b"] = "Ë";
	map_decode["Ã\x8c"] = "Ì";
	map_decode["Ã\x8d"] = "Í";
	map_decode["Ã\x8e"] = "Î";
	map_decode["Ã\x8f"] = "Ï";
	map_decode["Ã\x90"] = "Ð";
	map_decode["Ã\x91"] = "Ñ";
	map_decode["Ã\x92"] = "Ò";
	map_decode["Ã\x93"] = "Ó";
	map_decode["Ã\x94"] = "Ô";
	map_decode["Ã\x95"] = "Õ";
	map_decode["Ã\x96"] = "Ö";
	map_decode["Ã\x97"] = "×";
	map_decode["Ã\x98"] = "Ø";
	map_decode["Ã\x99"] = "Ù";
	map_decode["Ã\x9a"] = "Ú";
	map_decode["Ã\x9b"] = "Û";
	map_decode["Ã\x9c"] = "Ü";
	map_decode["Ã\x9d"] = "Ý";
	map_decode["Ã\x9e"] = "Þ";
	map_decode["Ã\x9f"] = "ß";
	map_decode["Ã "] = "à";
	map_decode["Ã¡"] = "á";
	map_decode["Ã¢"] = "â";
	map_decode["Ã£"] = "ã";
	map_decode["Ã¤"] = "ä";
	map_decode["Ã¥"] = "å";
	map_decode["Ã¦"] = "æ";
	map_decode["Ã§"] = "ç";
	map_decode["Ã¨"] = "è";
	map_decode["Ã©"] = "é";
	map_decode["Ãª"] = "ê";
	map_decode["Ã«"] = "ë";
	map_decode["Ã¬"] = "ì";
	map_decode["Ã­"] = "í";
	map_decode["Ã®"] = "î";
	map_decode["Ã¯"] = "ï";
	map_decode["Ã°"] = "ð";
	map_decode["Ã±"] = "ñ";
	map_decode["Ã²"] = "ò";
	map_decode["Ã³"] = "ó";
	map_decode["Ã´"] = "ô";
	map_decode["Ãµ"] = "õ";
	map_decode["Ã¶"] = "ö";
	map_decode["Ã·"] = "÷";
	map_decode["Ã¸"] = "ø";
	map_decode["Ã¹"] = "ù";
	map_decode["Ãº"] = "ú";
	map_decode["Ã»"] = "û";
	map_decode["Ã¼"] = "ü";
	map_decode["Ã½"] = "ý";
	map_decode["Ã¾"] = "þ";
	map_decode["Ã¿"] = "ÿ";
	// FIXME: This is just a hack to make Goran Mekic's name work.
	map_decode["Ä\x87"] = "c";
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

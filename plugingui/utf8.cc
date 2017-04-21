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
  map_encode["€"] = "Â€";
  map_encode[""] = "Â";
  map_encode["‚"] = "Â‚";
  map_encode["ƒ"] = "Âƒ";
  map_encode["„"] = "Â„";
  map_encode["…"] = "Â…";
  map_encode["†"] = "Â†";
  map_encode["‡"] = "Â‡";
  map_encode["ˆ"] = "Âˆ";
  map_encode["‰"] = "Â‰";
  map_encode["Š"] = "ÂŠ";
  map_encode["‹"] = "Â‹";
  map_encode["Œ"] = "ÂŒ";
  map_encode[""] = "Â";
  map_encode[""] = "Â";
  map_encode[""] = "Â";
  map_encode[""] = "Â";
  map_encode["‘"] = "Â‘";
  map_encode["’"] = "Â’";
  map_encode["“"] = "Â“";
  map_encode["”"] = "Â”";
  map_encode["•"] = "Â•";
  map_encode["–"] = "Â–";
  map_encode["—"] = "Â—";
  map_encode["˜"] = "Â˜";
  map_encode["™"] = "Â™";
  map_encode["š"] = "Âš";
  map_encode["›"] = "Â›";
  map_encode["œ"] = "Âœ";
  map_encode[""] = "Â";
  map_encode[""] = "Â";
  map_encode["Ÿ"] = "ÂŸ";
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
  map_encode["À"] = "Ã€";
  map_encode["Á"] = "Ã";
  map_encode["Â"] = "Ã‚";
  map_encode["Ã"] = "Ãƒ";
  map_encode["Ä"] = "Ã„";
  map_encode["Å"] = "Ã…";
  map_encode["Æ"] = "Ã†";
  map_encode["Ç"] = "Ã‡";
  map_encode["È"] = "Ãˆ";
  map_encode["É"] = "Ã‰";
  map_encode["Ê"] = "ÃŠ";
  map_encode["Ë"] = "Ã‹";
  map_encode["Ì"] = "ÃŒ";
  map_encode["Í"] = "Ã";
  map_encode["Î"] = "Ã";
  map_encode["Ï"] = "Ã";
  map_encode["Ğ"] = "Ã";
  map_encode["Ñ"] = "Ã‘";
  map_encode["Ò"] = "Ã’";
  map_encode["Ó"] = "Ã“";
  map_encode["Ô"] = "Ã”";
  map_encode["Õ"] = "Ã•";
  map_encode["Ö"] = "Ã–";
  map_encode["×"] = "Ã—";
  map_encode["Ø"] = "Ã˜";
  map_encode["Ù"] = "Ã™";
  map_encode["Ú"] = "Ãš";
  map_encode["Û"] = "Ã›";
  map_encode["Ü"] = "Ãœ";
  map_encode["İ"] = "Ã";
  map_encode["Ş"] = "Ã";
  map_encode["ß"] = "ÃŸ";
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
  map_encode["ğ"] = "Ã°";
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
  map_encode["ı"] = "Ã½";
  map_encode["ş"] = "Ã¾";
  map_encode["ÿ"] = "Ã¿";

  // Decode Map
  map_decode["Â€"] = "€";
  map_decode["Â"] = "";
  map_decode["Â‚"] = "‚";
  map_decode["Âƒ"] = "ƒ";
  map_decode["Â„"] = "„";
  map_decode["Â…"] = "…";
  map_decode["Â†"] = "†";
  map_decode["Â‡"] = "‡";
  map_decode["Âˆ"] = "ˆ";
  map_decode["Â‰"] = "‰";
  map_decode["ÂŠ"] = "Š";
  map_decode["Â‹"] = "‹";
  map_decode["ÂŒ"] = "Œ";
  map_decode["Â"] = "";
  map_decode["Â"] = "";
  map_decode["Â"] = "";
  map_decode["Â"] = "";
  map_decode["Â‘"] = "‘";
  map_decode["Â’"] = "’";
  map_decode["Â“"] = "“";
  map_decode["Â”"] = "”";
  map_decode["Â•"] = "•";
  map_decode["Â–"] = "–";
  map_decode["Â—"] = "—";
  map_decode["Â˜"] = "˜";
  map_decode["Â™"] = "™";
  map_decode["Âš"] = "š";
  map_decode["Â›"] = "›";
  map_decode["Âœ"] = "œ";
  map_decode["Â"] = "";
  map_decode["Â"] = "";
  map_decode["ÂŸ"] = "Ÿ";
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
  map_decode["Ã€"] = "À";
  map_decode["Ã"] = "Á";
  map_decode["Ã‚"] = "Â";
  map_decode["Ãƒ"] = "Ã";
  map_decode["Ã„"] = "Ä";
  map_decode["Ã…"] = "Å";
  map_decode["Ã†"] = "Æ";
  map_decode["Ã‡"] = "Ç";
  map_decode["Ãˆ"] = "È";
  map_decode["Ã‰"] = "É";
  map_decode["ÃŠ"] = "Ê";
  map_decode["Ã‹"] = "Ë";
  map_decode["ÃŒ"] = "Ì";
  map_decode["Ã"] = "Í";
  map_decode["Ã"] = "Î";
  map_decode["Ã"] = "Ï";
  map_decode["Ã"] = "Ğ";
  map_decode["Ã‘"] = "Ñ";
  map_decode["Ã’"] = "Ò";
  map_decode["Ã“"] = "Ó";
  map_decode["Ã”"] = "Ô";
  map_decode["Ã•"] = "Õ";
  map_decode["Ã–"] = "Ö";
  map_decode["Ã—"] = "×";
  map_decode["Ã˜"] = "Ø";
  map_decode["Ã™"] = "Ù";
  map_decode["Ãš"] = "Ú";
  map_decode["Ã›"] = "Û";
  map_decode["Ãœ"] = "Ü";
  map_decode["Ã"] = "İ";
  map_decode["Ã"] = "Ş";
  map_decode["ÃŸ"] = "ß";
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
  map_decode["Ã°"] = "ğ";
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
  map_decode["Ã½"] = "ı";
  map_decode["Ã¾"] = "ş";
  map_decode["Ã¿"] = "ÿ";
  // FIXME: This is just a hack to make Goran Mekic's name work.
  map_decode["Ä‡"] = "c";
}

std::string UTF8::fromLatin1(std::string const& s)
{
  std::string ret;

  for(int i = 0; i < (int)s.length(); i++) {
    std::string c;

    if((unsigned char)s[i] <= 0x7F) c = s.substr(i, 1);
    else c = map_encode[s.substr(i, 1)];

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
  for(int i = 0; i < (int)s.length(); i+=width) {
    if(/*(unsigned char)s[i]>=0x00&&*/(unsigned char)s[i] <= 0x7F) width = 1; // 00-7F	1 byte
    if((unsigned char)s[i] >= 0xC2 && (unsigned char)s[i] <= 0xDF) width = 2; // C2-DF	2 bytes
    if((unsigned char)s[i] >= 0xE0 && (unsigned char)s[i] <= 0xEF) width = 3; // E0-EF	3 bytes
    if((unsigned char)s[i] >= 0xF0 && (unsigned char)s[i] <= 0xF4) width = 4; // F0-F4	4 bytes

    std::string c;

    if(width == 1) c = s.substr(i, 1);
    else c = map_decode[s.substr(i, width)];

	// If c == "", the character wasn't found in the map.
	// Ignore this case for now and just push an empty string in this case.

    ret.append(c);
  }

  return ret;
}

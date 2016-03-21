/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            saxparser.cc
 *
 *  Tue Jul 22 16:26:22 CEST 2008
 *  Copyright 2008 Bent Bisballe Nyeng
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
#include "saxparser.h"

#include <stdio.h>
#include <string.h>
#include <hugin.hpp>

SAXParser::SAXParser()
{
	p = XML_ParserCreate(nullptr);
	if(!p) {
		ERR(sax, "Couldn't allocate memory for parser\n");
		// throw Exception(...); TODO
		return;
	}

	XML_SetUserData(p, this);
	XML_UseParserAsHandlerArg(p);
	XML_SetElementHandler(p, start_hndl, end_hndl);
	XML_SetCharacterDataHandler(p, character_hndl);
}

SAXParser::~SAXParser()
{
	XML_ParserFree(p);
}

int SAXParser::parse()
{
	DEBUG(sax, "parse()\n");

	std::string buf;
	int len;

	do {
		len = readData(buf, 32);
		if(len <= -1) {
			parseError("", 0, "Could not read data", 0);
			return 1;
		}
		if(!XML_Parse(p, (char*)buf.c_str(), len, len == 0)) {
			parseError(buf, len, XML_ErrorString(XML_GetErrorCode(p)),
				   (int)XML_GetCurrentLineNumber(p));
			return 1;
		}

		buf.clear();
	} while(len);

	return 0;
}

int SAXParser::parse(const std::string& buffer)
{
	DEBUG(sax, "parse(buffer %d bytes)\n", (int)buffer.length());

	if(!XML_Parse(p, buffer.c_str(), buffer.length(), true)) {
		parseError(buffer, buffer.length(),
			   XML_ErrorString(XML_GetErrorCode(p)),
			   (int)XML_GetCurrentLineNumber(p));
		return 1;
	}

	return 0;
}

void SAXParser::parseError(const std::string& buf, std::size_t len, const std::string& error, std::size_t lineno)
{
	fprintf(stderr, "SAXParser error at line %d: %s\n", (int)lineno, error.c_str());
	fprintf(stderr, "Buffer %u bytes: \n[\n", (int)len);

	fwrite((char*)buf.c_str(), len, 1, stderr);

	fprintf(stderr, "\n]\n");
	fflush(stderr);
}

void SAXParser::character_hndl(void* p, const XML_Char* s, int len)
{
	SAXParser* parser = (SAXParser*)XML_GetUserData(p);
	std::string chars(s, len);
	parser->characterData(chars);
}

void SAXParser::start_hndl(void* p, const char* el, const char** attr)
{
	SAXParser* parser = (SAXParser*)XML_GetUserData(p);

	// Convert to comfy C++ values...
	attr_t attributes;

	while(*attr) {
		std::string at_name = *attr++;
		std::string at_value = *attr++;

		attributes.emplace(at_name, at_value);
	}

	parser->startTag(std::string(el), attributes);
}

void SAXParser::end_hndl(void* p, const char* el)
{
	SAXParser* parser = (SAXParser*)XML_GetUserData(p);
	parser->endTag(std::string(el));
}

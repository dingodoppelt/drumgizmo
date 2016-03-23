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

#include <sstream>
#include <iostream>

#include <hugin.hpp>

SAXParser::SAXParser()
{
	parser = XML_ParserCreate(nullptr);
	if(!parser)
	{
		ERR(sax, "Couldn't allocate memory for parser\n");
		// throw Exception(...); TODO
		return;
	}

	XML_SetUserData(parser, this);
	XML_UseParserAsHandlerArg(parser);
	XML_SetElementHandler(parser, SAXParser::startHandler, SAXParser::endHandler);
	XML_SetCharacterDataHandler(parser, SAXParser::characterHandler);
}

SAXParser::~SAXParser()
{
	XML_ParserFree(parser);
}

int SAXParser::parseFile(const std::string& filename)
{
	if(filename.empty())
	{
		return 0;
	}

	std::ifstream file(filename, std::ifstream::in);

	if(!file.is_open())
	{
		return 1;
	}

	std::stringstream ss;
	ss << file.rdbuf();
	std::string str = ss.str();

	return parseString(str, filename);
}

int SAXParser::parseString(const std::string& str,
                           const std::string& xml_source_name)
{
	DEBUG(sax, "parse(buffer %d bytes)\n", (int)str.length());

	if(!XML_Parse(parser, str.c_str(), str.length(), true))
	{
		parseError(str, XML_ErrorString(XML_GetErrorCode(parser)),
		           xml_source_name, (int)XML_GetCurrentLineNumber(parser));
		return 1;
	}

	return 0;
}

void SAXParser::parseError(const std::string& buf, const std::string& error,
                           const std::string& xml_source_name,
                           std::size_t lineno)
{
	std::cerr << "SAXParser error trying to parse from source: " <<
		xml_source_name << "\n";
	std::cerr << "At line " << lineno << ": " << error << "\n";
	std::cerr << "Buffer " << buf.size() << " bytes: \n[\n";
	std::cerr << buf;
	std::cerr << "\n]" << std::endl;
}

void SAXParser::characterHandler(void* parser, const XML_Char* cData, int len)
{
	SAXParser* sax_parser = (SAXParser*)XML_GetUserData(parser);
	std::string chars(cData, len);
	sax_parser->characterData(chars);
}

void SAXParser::startHandler(void* parser, const char* el, const char** attr)
{
	SAXParser* sax_parser = (SAXParser*)XML_GetUserData(parser);

	// Convert to comfy C++ values...
	attr_t attributes;

	while(*attr)
	{
		std::string at_name = *attr++;
		std::string at_value = *attr++;

		attributes.emplace(at_name, at_value);
	}

	sax_parser->startTag(std::string(el), attributes);
}

void SAXParser::endHandler(void* parser, const char* el)
{
	SAXParser* sax_parser = (SAXParser*)XML_GetUserData(parser);
	sax_parser->endTag(std::string(el));
}

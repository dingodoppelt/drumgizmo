/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            saxparser.h
 *
 *  Tue Jul 22 16:26:21 CEST 2008
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
#pragma once

#include <string>
#include <map>
#include <expat.h>
#include <fstream>

class SAXParser {
public:
	SAXParser();
	virtual ~SAXParser();

	//! Parses the data from the file.
	virtual int parseFile(const std::string& filename);

	//! Parses all the data in the buffer.
	virtual int parseString(const std::string& str, const std::string& xml_source_name = "");

protected:
	using attr_t = std::map<std::string, std::string>;

	virtual void characterData(const std::string& data) {}
	virtual void startTag(const std::string& name, const attr_t& attr) {}
	virtual void endTag(const std::string& name) {}
	virtual void parseError(const std::string& buf, const std::string& error, const std::string& xml_source_name, std::size_t lineno);

private:
	XML_Parser p;
	std::string filename;

	static void character_hndl(void* p, const XML_Char* s, int len);
	static void start_hndl(void* p, const char* el, const char** attr);
	static void end_hndl(void* p, const char* el);
};

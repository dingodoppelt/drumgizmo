/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            enginefactory.h
 *
 *  Mi 20. Jan 10:46:07 CET 2016
 *  Copyright 2016 Christian Glöckner
 *  cgloeckner@freenet.de
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
#pragma once
#include <memory>
#include "cpp11fix.h" // required for c++11

#include "audioinputengine.h"
#include "audiooutputengine.h"

// todo: ifdef jack enabled
#include "jackclient.h"

using JackClientPtr = std::unique_ptr<JackClient>;
using InputEnginePtr = std::unique_ptr<AudioInputEngine>;
using OutputEnginePtr = std::unique_ptr<AudioOutputEngine>;

InputEnginePtr createInputEngine(JackClientPtr& jack, std::string const & name);
OutputEnginePtr createOutputEngine(JackClientPtr& jack, std::string const & name);

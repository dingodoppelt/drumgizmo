/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            settings.h
 *
 *  Tue Mar 22 10:59:46 CET 2016
 *  Copyright 2016 Christian Glöckner
 *  cgloeckner@freenet.de
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
#pragma once

#include <atomic>
#include <string>
#include <cassert>

#include "atomic.h"

//! Engine settings
struct Settings
{
	Atomic<bool> enable_velocity_modifier;
	Atomic<float> velocity_modifier_falloff;
	Atomic<float> velocity_modifier_weight;

	Atomic<bool> enable_velocity_randomiser;
	Atomic<float> velocity_randomiser_weight;

	Atomic<double> samplerate;

	Atomic<bool> enable_resampling;

	Atomic<int> number_of_files;
	Atomic<int> number_of_files_loaded;
	Atomic<std::string> current_file;
};

//! Getter utility class.
template <typename T> class SettingRef
{
public:
	SettingRef(Atomic<T>& value)
		: value(value)
	{
		// string isn't lock free either
		assert((std::is_same<T, std::string>::value || value.is_lock_free()));
	}

	bool hasChanged()
	{
		T tmp = cache;
		cache.exchange(value);
		return tmp == cache;
	}

	T getValue() const
	{
		return cache;
	}

private:
	Atomic<T>& value;
	Atomic<T> cache;
};

//! Combined getter class.
struct SettingsGetter
{
	SettingRef<bool> enable_velocity_modifier;
	SettingRef<float> velocity_modifier_falloff;
	SettingRef<float> velocity_modifier_weight;

	SettingRef<bool> enable_velocity_randomiser;
	SettingRef<float> velocity_randomiser_weight;

	SettingRef<double> samplerate;

	SettingRef<bool> enable_resampling;

	SettingRef<int> number_of_files;
	SettingRef<int> number_of_files_loaded;
	SettingRef<std::string> current_file;

	SettingsGetter(Settings& settings)
		: enable_velocity_modifier{settings.enable_velocity_modifier}
		, velocity_modifier_falloff{settings.velocity_modifier_falloff}
		, velocity_modifier_weight{settings.velocity_modifier_weight}
		, enable_velocity_randomiser{settings.enable_velocity_randomiser}
		, velocity_randomiser_weight{settings.velocity_randomiser_weight}
		, samplerate{settings.samplerate}
		, enable_resampling{settings.enable_resampling}
		, number_of_files{settings.number_of_files}
		, number_of_files_loaded{settings.number_of_files_loaded}
		, current_file{settings.current_file}
	{
	}
};

// lovely reminder: NO, GLOCKE. NOOOO!!
/*
enum class IntParams {
	Foo = 0
};

struct Settings
{
	std::array<std::atomic<int>, 5> ints;

	Settings()
		: ints{}
	{
		//get(IntParams::Foo).store(3);
	}

	std::atomic<int>& get(IntParams param)
	{
		return ints[(size_t)param];
	}
};

struct SettingsGetter
{
	std::vector<SettingRef<int>> ints;

	SettingsGetter(Settings& parent)
	{
		for(auto& atomic: parent.ints)
		{
			ints.emplace_back(atomic);
		}
	}
};
*/

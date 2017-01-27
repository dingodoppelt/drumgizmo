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
#include <limits>

#include <cassert>

#include "atomic.h"
#include "notifier.h"

enum class LoadStatus : unsigned int
{
	Idle,
	Loading,
	Done,
	Error
};

//! Engine settings
struct Settings
{
	Atomic<std::string> drumkit_file{""};
	Atomic<LoadStatus> drumkit_load_status{LoadStatus::Idle};

	//! The maximum amount of memory in bytes that the AudioCache
	//! is allowed to use for preloading
	//! The default std::numeric_limits<std::size_t>::max() means "unlimited"
	//Atomic<std::size_t> cache_upper_limit{std::numeric_limits<std::size_t>::max()};
	Atomic<std::size_t> disk_cache_upper_limit{1024*1024*1024};
	Atomic<bool> disk_cache_enable{true};

	Atomic<std::string> midimap_file{""};
	Atomic<LoadStatus> midimap_load_status{LoadStatus::Idle};

	Atomic<bool> enable_velocity_modifier{true};
	Atomic<float> velocity_modifier_falloff{0.5f};
	Atomic<float> velocity_modifier_weight{0.25f};

	Atomic<bool> enable_velocity_randomiser{false};
	Atomic<float> velocity_randomiser_weight{0.1f};

	Atomic<double> samplerate{44100.0};

	Atomic<bool> enable_resampling{true};

	Atomic<std::size_t> number_of_files{0};
	Atomic<std::size_t> number_of_files_loaded{0};
	Atomic<std::string> current_file{""};
};

//! Settings getter class.
struct SettingsGetter
{
	SettingRef<std::string> drumkit_file;
	SettingRef<LoadStatus> drumkit_load_status;

	SettingRef<std::size_t> disk_cache_upper_limit;
	SettingRef<bool> disk_cache_enable;

	SettingRef<std::string> midimap_file;
	SettingRef<LoadStatus> midimap_load_status;

	SettingRef<bool> enable_velocity_modifier;
	SettingRef<float> velocity_modifier_falloff;
	SettingRef<float> velocity_modifier_weight;

	SettingRef<bool> enable_velocity_randomiser;
	SettingRef<float> velocity_randomiser_weight;

	SettingRef<double> samplerate;

	SettingRef<bool> enable_resampling;

	SettingRef<std::size_t> number_of_files;
	SettingRef<std::size_t> number_of_files_loaded;
	SettingRef<std::string> current_file;

	SettingsGetter(Settings& settings)
		: drumkit_file(settings.drumkit_file)
		, drumkit_load_status(settings.drumkit_load_status)
		, disk_cache_upper_limit(settings.disk_cache_upper_limit)
		, disk_cache_enable(settings.disk_cache_enable)
		, midimap_file(settings.midimap_file)
		, midimap_load_status(settings.midimap_load_status)
		, enable_velocity_modifier{settings.enable_velocity_modifier}
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

//! Settings change notifier class.
class SettingsNotifier
{
public:
	Notifier<std::string> drumkit_file;
	Notifier<LoadStatus> drumkit_load_status;

	Notifier<std::size_t> disk_cache_upper_limit;
	Notifier<bool> disk_cache_enable;

	Notifier<std::string> midimap_file;
	Notifier<LoadStatus> midimap_load_status;

	Notifier<bool> enable_velocity_modifier;
	Notifier<float> velocity_modifier_falloff;
	Notifier<float> velocity_modifier_weight;

	Notifier<bool> enable_velocity_randomiser;
	Notifier<float> velocity_randomiser_weight;

	Notifier<double> samplerate;

	Notifier<bool> enable_resampling;

	Notifier<std::size_t> number_of_files;
	Notifier<std::size_t> number_of_files_loaded;
	Notifier<std::string> current_file;

	void evaluate()
	{
#define EVAL(x) if(settings.x.hasChanged()) { x(settings.x.getValue()); }

		EVAL(drumkit_file);
		EVAL(drumkit_load_status);

		EVAL(disk_cache_upper_limit);
		EVAL(disk_cache_enable);

		EVAL(midimap_file);
		EVAL(midimap_load_status);

		EVAL(enable_velocity_modifier);
		EVAL(velocity_modifier_falloff);
		EVAL(velocity_modifier_weight);

		EVAL(enable_velocity_randomiser);
		EVAL(velocity_randomiser_weight);

		EVAL(samplerate);

		EVAL(enable_resampling);

		EVAL(number_of_files);
		EVAL(number_of_files_loaded);
		EVAL(current_file);
	}

	SettingsNotifier(Settings& settings)
		: settings(settings)
	{
	}

private:
	SettingsGetter settings;
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

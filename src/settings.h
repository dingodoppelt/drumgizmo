/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            settings.h
 *
 *  Tue Mar 22 10:59:46 CET 2016
 *  Copyright 2016 Christian Gl�ckner
 *  cgloeckner@freenet.de
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

#include <atomic>
#include <cassert>

template <typename T> class SettingRef
{
public:
	SettingRef(std::atomic<T>& value)
		: value{value}
		, cache{}
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
	std::atomic<T>& value;
	std::atomic<T> cache;
};

struct Settings
{
	std::atomic<bool> enable_velocity_modifier;
	std::atomic<float> velocity_modifier_falloff;
	std::atomic<float> velocity_modifier_weight;

	std::atomic<bool> enable_velocity_randomiser;
	std::atomic<float> velocity_randomiser_weight;

	std::atomic<int> samplerate;

	std::atomic<bool> enable_resampling;
};

struct SettingsGetter
{
	SettingRef<bool> enable_velocity_modifier;
	SettingRef<float> velocity_modifier_falloff;
	SettingRef<float> velocity_modifier_weight;

	SettingRef<bool> enable_velocity_randomiser;
	SettingRef<float> velocity_randomiser_weight;

	SettingRef<int> samplerate;

	SettingRef<bool> enable_resampling;

	SettingsGetter(Settings& settings)
		: enable_velocity_modifier{settings.enable_velocity_modifier}
		, velocity_modifier_falloff{settings.velocity_modifier_falloff}
		, velocity_modifier_weight{settings.velocity_modifier_weight}
		, enable_velocity_randomiser{settings.enable_velocity_randomiser}
		, velocity_randomiser_weight{settings.velocity_randomiser_weight}
		, samplerate{settings.samplerate}
		, enable_resampling{settings.enable_resampling}
	{
	}
};

// lovely reminder: NO, GLOCKE. NOOOO!!
/*
enum class IntParams {
    Foo = 0
};

struct Settings {
    std::array<std::atomic<int>, 5> ints;

    Settings()
        : ints{} {
        //get(IntParams::Foo).store(3);
    }

    std::atomic<int>& get(IntParams param) {
        return ints[(size_t)param];
    }
};

struct SettingsGetter {
    std::vector<SettingRef<int>> ints;

    SettingsGetter(Settings& parent) {
        for (auto& atomic: parent.ints) {
            ints.emplace_back(atomic);
        }
    }
};
*/
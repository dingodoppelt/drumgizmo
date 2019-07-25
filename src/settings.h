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
	Atomic<std::string> drumkit_name{""};
	Atomic<std::string> drumkit_description{""};
	Atomic<std::string> drumkit_version{""};
	Atomic<std::size_t> drumkit_samplerate{44100};

	//! The maximum amount of memory in bytes that the AudioCache
	//! is allowed to use for preloading. Default is 1GB.
	Atomic<std::size_t> disk_cache_upper_limit{1024 * 1024 * 1024};

	//! The optimal read chunk size from the disk.
	Atomic<std::size_t> disk_cache_chunk_size{1024 * 1024};

	Atomic<bool> disk_cache_enable{true};

	Atomic<std::size_t> number_of_underruns{0};

	//! Increment this in order to invoke a reload of the current drumkit.
	Atomic<std::size_t> reload_counter{0};

	Atomic<std::string> midimap_file{""};
	Atomic<LoadStatus> midimap_load_status{LoadStatus::Idle};

	Atomic<bool> enable_velocity_modifier{true};

	static float constexpr velocity_modifier_falloff_default = 0.5f;
	static float constexpr velocity_modifier_weight_default = 0.25f;
	static float constexpr velocity_stddev_default = 1.0f;
	static float constexpr sample_selection_f_close_default = .5f;
	static float constexpr sample_selection_f_diverse_default = .2f;
	static float constexpr sample_selection_f_random_default = .1f;
	Atomic<float> velocity_modifier_falloff{velocity_modifier_falloff_default};
	Atomic<float> velocity_modifier_weight{velocity_modifier_weight_default};
	Atomic<float> velocity_stddev{velocity_stddev_default};
	Atomic<float> sample_selection_f_close{sample_selection_f_close_default};
	Atomic<float> sample_selection_f_diverse{sample_selection_f_diverse_default};
	Atomic<float> sample_selection_f_random{sample_selection_f_random_default};

	//! Control number of times to retry sample selection as long as the sample
	//! is the same one as the last one.
	//! 0: will do no retries, ie. just use the first sample found.
	// FIXME: remove when new sample algorithm is introduced and also remove other occurences
	static std::size_t constexpr sample_selection_retry_count_default = 3;
	Atomic<std::size_t> sample_selection_retry_count{sample_selection_retry_count_default};

	// Current velocity offset - for UI
	Atomic<float> velocity_modifier_current{1.0f};

	Atomic<bool> enable_velocity_randomiser{false};
	Atomic<float> velocity_randomiser_weight{0.1f};

	Atomic<float> samplerate{44100.0};
	Atomic<std::size_t> buffer_size{1024}; // Only used to show in the UI.

	Atomic<bool> enable_resampling{true};
	Atomic<bool> resamplig_recommended{false};

	Atomic<std::size_t> number_of_files{0};
	Atomic<std::size_t> number_of_files_loaded{0};
	Atomic<std::string> current_file{""};

	Atomic<bool> enable_bleed_control{false};
	Atomic<float> master_bleed{1.0f};
	Atomic<bool> has_bleed_control{false};

	Atomic<bool> enable_latency_modifier{false};

	//! Maximum "early hits" introduces latency in milliseconds.
	Atomic<float> latency_max_ms{150.0f};

	//! 0 := on-beat
	//! positive := laid back
	//! negative := up-beat
	//! Same range is [-100; 100] ms
	static float constexpr latency_laid_back_ms_default = 0.0f;
	Atomic<float> latency_laid_back_ms{latency_laid_back_ms_default};

	//! 0.0 := Robot
	//! 2.0 := Good drummer
	//! 4.0 := Decent drummer
	//! 6.0 := Decent drummer on a bad day
	//! 8.0 := Bad drummer
	//! 10.0 := Bad and drunk drummer
	static float constexpr latency_stddev_default = 2.0f;
	Atomic<float> latency_stddev{latency_stddev_default};

	//! Regain on-beat position.
	//! 0: instantaniously
	//! 1: never
	static float constexpr latency_regain_default = 0.9f;
	Atomic<float> latency_regain{latency_regain_default};

	// Current latency offset in ms - for UI
	Atomic<float> latency_current{0};

	Atomic<std::size_t> audition_counter{0};
	Atomic<std::string> audition_instrument;
	Atomic<float> audition_velocity;

	// Notify UI about load errors
	Atomic<std::string> load_status_text;
};

//! Settings getter class.
struct SettingsGetter
{
	SettingRef<std::string> drumkit_file;
	SettingRef<LoadStatus> drumkit_load_status;
	SettingRef<std::string> drumkit_name;
	SettingRef<std::string> drumkit_description;
	SettingRef<std::string> drumkit_version;
	SettingRef<std::size_t> drumkit_samplerate;

	SettingRef<std::size_t> disk_cache_upper_limit;
	SettingRef<std::size_t> disk_cache_chunk_size;
	SettingRef<bool> disk_cache_enable;
	SettingRef<std::size_t> number_of_underruns;
	SettingRef<std::size_t> reload_counter;

	SettingRef<std::string> midimap_file;
	SettingRef<LoadStatus> midimap_load_status;

	SettingRef<bool> enable_velocity_modifier;
	SettingRef<float> velocity_modifier_falloff;
	SettingRef<float> velocity_modifier_weight;
	SettingRef<float> velocity_stddev;
	SettingRef<float> sample_selection_f_close;
	SettingRef<float> sample_selection_f_diverse;
	SettingRef<float> sample_selection_f_random;

	SettingRef<std::size_t> sample_selection_retry_count;

	SettingRef<float> velocity_modifier_current;

	SettingRef<bool> enable_velocity_randomiser;
	SettingRef<float> velocity_randomiser_weight;

	SettingRef<float> samplerate;
	SettingRef<std::size_t> buffer_size;

	SettingRef<bool> enable_resampling;
	SettingRef<bool> resamplig_recommended;

	SettingRef<std::size_t> number_of_files;
	SettingRef<std::size_t> number_of_files_loaded;
	SettingRef<std::string> current_file;

	SettingRef<bool> enable_bleed_control;
	SettingRef<float> master_bleed;
	SettingRef<bool> has_bleed_control;

	SettingRef<bool> enable_latency_modifier;
	SettingRef<float> latency_max_ms;
	SettingRef<float> latency_laid_back_ms;
	SettingRef<float> latency_stddev;
	SettingRef<float> latency_regain;
	SettingRef<float> latency_current;

	SettingRef<std::size_t> audition_counter;
	SettingRef<std::string> audition_instrument;
	SettingRef<float> audition_velocity;

	SettingRef<std::string> load_status_text;

	SettingsGetter(Settings& settings)
		: drumkit_file(settings.drumkit_file)
		, drumkit_load_status(settings.drumkit_load_status)
		, drumkit_name(settings.drumkit_name)
		, drumkit_description(settings.drumkit_description)
		, drumkit_version(settings.drumkit_version)
		, drumkit_samplerate(settings.drumkit_samplerate)
		, disk_cache_upper_limit(settings.disk_cache_upper_limit)
		, disk_cache_chunk_size(settings.disk_cache_chunk_size)
		, disk_cache_enable(settings.disk_cache_enable)
		, number_of_underruns(settings.number_of_underruns)
		, reload_counter(settings.reload_counter)
		, midimap_file(settings.midimap_file)
		, midimap_load_status(settings.midimap_load_status)
		, enable_velocity_modifier{settings.enable_velocity_modifier}
		, velocity_modifier_falloff{settings.velocity_modifier_falloff}
		, velocity_modifier_weight{settings.velocity_modifier_weight}
		, velocity_stddev{settings.velocity_stddev}
		, sample_selection_f_close{settings.sample_selection_f_close}
		, sample_selection_f_diverse{settings.sample_selection_f_diverse}
		, sample_selection_f_random{settings.sample_selection_f_random}
		, sample_selection_retry_count(settings.sample_selection_retry_count)
		, velocity_modifier_current{settings.velocity_modifier_current}
		, enable_velocity_randomiser{settings.enable_velocity_randomiser}
		, velocity_randomiser_weight{settings.velocity_randomiser_weight}
		, samplerate{settings.samplerate}
		, buffer_size(settings.buffer_size)
		, enable_resampling{settings.enable_resampling}
		, resamplig_recommended{settings.resamplig_recommended}
		, number_of_files{settings.number_of_files}
		, number_of_files_loaded{settings.number_of_files_loaded}
		, current_file{settings.current_file}
		, enable_bleed_control{settings.enable_bleed_control}
		, master_bleed{settings.master_bleed}
		, has_bleed_control{settings.has_bleed_control}
		, enable_latency_modifier{settings.enable_latency_modifier}
		, latency_max_ms{settings.latency_max_ms}
		, latency_laid_back_ms{settings.latency_laid_back_ms}
		, latency_stddev{settings.latency_stddev}
		, latency_regain{settings.latency_regain}
		, latency_current{settings.latency_current}
		, audition_counter{settings.audition_counter}
		, audition_instrument{settings.audition_instrument}
		, audition_velocity{settings.audition_velocity}
		, load_status_text{settings.load_status_text}
	{
	}
};

//! Settings change notifier class.
class SettingsNotifier
{
public:
	Notifier<std::string> drumkit_file;
	Notifier<LoadStatus> drumkit_load_status;
	Notifier<std::string> drumkit_name;
	Notifier<std::string> drumkit_description;
	Notifier<std::string> drumkit_version;
	Notifier<std::size_t> drumkit_samplerate;

	Notifier<std::size_t> disk_cache_upper_limit;
	Notifier<std::size_t> disk_cache_chunk_size;
	Notifier<bool> disk_cache_enable;
	Notifier<std::size_t> number_of_underruns;
	Notifier<std::size_t> reload_counter;

	Notifier<std::string> midimap_file;
	Notifier<LoadStatus> midimap_load_status;

	Notifier<bool> enable_velocity_modifier;
	Notifier<float> velocity_modifier_falloff;
	Notifier<float> velocity_modifier_weight;
	Notifier<float> velocity_stddev;
	Notifier<float> sample_selection_f_close;
	Notifier<float> sample_selection_f_diverse;
	Notifier<float> sample_selection_f_random;
	Notifier<std::size_t> sample_selection_retry_count;
	Notifier<float> velocity_modifier_current;

	Notifier<bool> enable_velocity_randomiser;
	Notifier<float> velocity_randomiser_weight;

	Notifier<float> samplerate;
	Notifier<std::size_t> buffer_size;

	Notifier<bool> enable_resampling;
	Notifier<bool> resamplig_recommended;

	Notifier<std::size_t> number_of_files;
	Notifier<std::size_t> number_of_files_loaded;
	Notifier<std::string> current_file;

	Notifier<bool> enable_bleed_control;
	Notifier<float> master_bleed;
	Notifier<bool> has_bleed_control;

	Notifier<bool> enable_latency_modifier;
	Notifier<float> latency_max_ms;
	Notifier<float> latency_laid_back_ms;
	Notifier<float> latency_stddev;
	Notifier<float> latency_regain;
	Notifier<float> latency_current;

	Notifier<std::size_t> audition_counter;
	Notifier<std::string> audition_instrument;
	Notifier<int> audition_velocity;

	Notifier<std::string> load_status_text;

	void evaluate()
	{
#define EVAL(x) if(settings.x.hasChanged()) { x(settings.x.getValue()); }

		EVAL(drumkit_file);
		EVAL(drumkit_load_status);
		EVAL(drumkit_name);
		EVAL(drumkit_description);
		EVAL(drumkit_version);
		EVAL(drumkit_samplerate);

		EVAL(disk_cache_upper_limit);
		EVAL(disk_cache_chunk_size);
		EVAL(disk_cache_enable);
		EVAL(number_of_underruns);
		EVAL(reload_counter);

		EVAL(midimap_file);
		EVAL(midimap_load_status);

		EVAL(enable_velocity_modifier);
		EVAL(velocity_modifier_falloff);
		EVAL(velocity_modifier_weight);
		EVAL(velocity_stddev);
		EVAL(sample_selection_f_close);
		EVAL(sample_selection_f_diverse);
		EVAL(sample_selection_f_random);
		EVAL(sample_selection_retry_count);
		EVAL(velocity_modifier_current);

		EVAL(enable_velocity_randomiser);
		EVAL(velocity_randomiser_weight);

		EVAL(samplerate);
		EVAL(buffer_size);

		EVAL(enable_resampling);
		EVAL(resamplig_recommended);

		EVAL(number_of_files);
		EVAL(number_of_files_loaded);
		EVAL(current_file);

		EVAL(enable_bleed_control);
		EVAL(master_bleed);
		EVAL(has_bleed_control);

		EVAL(enable_latency_modifier);
		EVAL(latency_max_ms);
		EVAL(latency_laid_back_ms);
		EVAL(latency_stddev);
		EVAL(latency_regain);
		EVAL(latency_current);

		EVAL(audition_counter);
		EVAL(audition_instrument);
		EVAL(audition_velocity);

		EVAL(load_status_text);
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

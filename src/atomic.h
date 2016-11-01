/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            atomic.h
 *
 *  Wed Mar 23 09:15:05 CET 2016
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

#include <type_traits>
#include <atomic>

#include <mutex>
#include "mutex.h"

template <typename T, typename = void>
class Atomic;

// use std::atomic if possible
template <typename T>
class Atomic<T, typename std::enable_if<std::is_pod<T>::value>::type>
	: public std::atomic<T>
{
public:
	// inherit methods
	using std::atomic<T>::atomic;
	using std::atomic<T>::operator=;
};

// else work around it using a mutex
template <typename T>
class Atomic<T, typename std::enable_if<!std::is_pod<T>::value>::type>
{
public:
	using self_type =
		Atomic<T, typename std::enable_if<!std::is_pod<T>::value>::type>;

	Atomic()
		: data{}
		, mutex{}
	{
	}

	Atomic(T data)
		: data{std::move(data)}
		, mutex{}
	{
	}

	Atomic(self_type const & other)
		: data{}
		, mutex{}
	{
		std::lock_guard<std::mutex> lock{other.mutex};
		data = other.data;
	}

	Atomic(self_type&& other)
		: data{}
		, mutex{}
	{
		std::lock_guard<std::mutex> lock{other.mutex};
		std::swap(data, other.data);
	}

	T operator=(T data)
	{
		std::lock_guard<std::mutex> lock{mutex};
		this->data = std::move(data);
		return this->data;
	}

	operator T() const
	{
		return load();
	}

	bool is_lock_free() const
	{
		return false;
	}

	void store(T data)
	{
		std::lock_guard<std::mutex> lock{mutex};
		this->data = std::move(data);
	}

	T load() const {
		std::lock_guard<std::mutex> lock{mutex};
		return data;
	}

	T exchange(T data){
		std::lock_guard<std::mutex> lock{mutex};
		std::swap(data, this->data);
		return data;
	}

	bool operator==(const T& other) const
	{
		std::lock_guard<std::mutex> lock{mutex};
		return other == data;
	}

	bool operator!=(const T& other) const
	{
		std::lock_guard<std::mutex> lock{mutex};
		return !(other == data);
	}

	bool operator==(const Atomic<T>& other) const
	{
		std::lock_guard<std::mutex> lock{mutex};
		return other.load() == data;
	}

	bool operator!=(const Atomic<T>& other) const
	{
		std::lock_guard<std::mutex> lock{mutex};
		return !(other.load() == data);
	}

private:
	T data;
	mutable std::mutex mutex;
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

		if(firstAccess)
		{
			firstAccess = false;
			return true;
		}

		return cache != tmp;
	}

	T getValue() const
	{
		return cache;
	}

private:
	bool firstAccess{true};
	Atomic<T>& value;
	Atomic<T> cache;
};

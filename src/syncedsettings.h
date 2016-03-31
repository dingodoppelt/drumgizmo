/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            syncedsettings.h
 *
 *  Thu Mar 31 09:23:27 CEST 2016
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

#include <mutex>

template <typename T> class Group;

template <typename T> class Accessor
{
private:
	std::lock_guard<std::mutex> lock;

public:
	Accessor(Group<T>& parent)
		: lock{parent.mutex}
		, data{parent.data}
	{
	}

	T& data;
};

template <typename T> class Group
{
private:
	friend class Accessor<T>;

	mutable std::mutex mutex;
	T data;

public:
	Group()
		: mutex{}
		, data{}
	{
	}

	Group(T const& data)
		: mutex{}
		, data{data}
	{
	}

	Group(T&& data)
		: mutex{}
		, data{std::move(data)}
	{
	}

	Group(Group<T> const& other)
		: mutex{}
		, data{}
	{
		std::lock_guard<std::mutex> lock{other.mutex};
		data = other.data;
	}

	Group(Group<T>&& other)
		: mutex{}
		, data{}
	{
		std::lock_guard<std::mutex> lock{other.mutex};
		std::swap(data, other.data);
	}

	Group<T>& operator=(const Group<T>& other)
	{
		if (*this != &other)
		{
			std::lock_guard<std::mutex> lock{mutex};
			std::lock_guard<std::mutex> lock2{other.mutex};
			data = other.data;
		}
		return *this;
	}
	
	Group<T>& operator=(Group<T>&& other)
	{
		if (*this != &other)
		{
			std::lock_guard<std::mutex> lock{mutex};
			std::lock_guard<std::mutex> lock2{other.mutex};
			std::swap(data, other.data);
		}
		return *this;
	}

	operator T() const
	{
		std::lock_guard<std::mutex> lock{mutex};
		return data;
	}
};

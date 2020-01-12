/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            id.h
 *
 *  Sun 16 Feb 2020 04:17:19 PM CET
 *  Copyright 2020 André Nusser
 *  andre.nusser@googlemail.com
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

#include <cstdint>
#include <functional>
#include <limits>

// Typesafe ID class such that there are compiler errors if different IDs are
// mixed. The template parameter T is just there to assure this behavior.
// Additionally, we have a member function which can check for validity.
template <typename T>
struct ID
{
public:
	using IDType = uint32_t;
	static constexpr IDType invalid_value = std::numeric_limits<IDType>::max();

	ID(IDType id = invalid_value) : id(id) {}

	operator IDType() const { return id; }
	IDType operator+(ID<T> other) const { return id + other.id; }
	IDType operator+(int offset) const { return id + offset; }
	IDType operator+(size_t offset) const { return id + offset; }
	IDType operator-(ID<T> other) const { return id - other.id; }
	IDType operator-(int offset) const { return id - offset; }
	IDType operator/(int div) const { return id/div; }
	IDType operator+=(ID<T> other) { return id += other.id; }
	IDType operator-=(ID<T> other) { return id -= other.id; }
	IDType operator=(ID<T> other) { return id = other.id; }
	IDType operator++() { return ++id; }
	IDType operator--() { return --id; }
	bool operator!=(ID<T> other) const { return id != other.id; }

	bool valid() const { return id != invalid_value; }
	void invalidate() { id = invalid_value; }

private:
	IDType id;
};

// define custom hash function to be able to use IDs with maps/sets
namespace std
{

template <typename T>
struct hash<ID<T>>
{
	using IDType = typename ID<T>::IDType;
	std::size_t operator()(ID<T> const& id) const noexcept
	{
		return std::hash<IDType>()(id);
	}
};

} // std

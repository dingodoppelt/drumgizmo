/* -*- Mode: c++ -*- */
/***************************************************************************
 *            memory_heap.h
 *
 *  Sun Jan 19 13:49:56 CET 2020
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

#include <vector>

// The purpose of this class is to have a heap for objects of a certain type.
// We want to avoid using the general "new"/"delete" memory allocation as this
// is problematic for real-time as well as slow. The underlying container still
// is a vector to handle the case when for some reason we need more memory
// than initially anticipated. Instead of throwing errors and giving up, we can
// then at least allocate new memory and just hope for the best.
//
// The members add, get, remove correspond to new, dereference, delete. Thus, we
// first add an object and this gives us and index. With this index we can then
// retrieve the object until we remove it. As soon as it is removed, you shouldn't
// use the index anymore to retrieve it (similarly, as you shouldn't use a pointer
// after calling delete on it).
template <typename T>
class MemoryHeap
{
public:
	using Index = std::size_t;
	using Indices = std::vector<Index>;

	MemoryHeap() = default;
	MemoryHeap(std::size_t size)
	{
		memory.reserve(size);
		free_indices.reserve(size);
	}

	template <typename... Args>
	Index emplace(Args&&... args);

	Index add(const T& element);
	T& get(Index index);
	const T& get(Index index) const;
	void remove(Index index);

private:
	std::vector<T> memory;
	Indices free_indices;
};

template <typename T>
auto MemoryHeap<T>::add(const T& element) -> Index
{
	if (free_indices.empty())
	{
		memory.push_back(element);
		return memory.size()-1;
	}

	auto free_index = free_indices.back();
	free_indices.pop_back();
	memory[free_index] = element;
	return free_index;
}

template <typename T>
template <typename... Args>
auto MemoryHeap<T>::emplace(Args&&... args) -> Index
{
	if (free_indices.empty())
	{
		memory.emplace_back(std::forward<Args>(args)...);
		return memory.size()-1;
	}

	auto free_index = free_indices.back();
	free_indices.pop_back();
	memory[free_index] = T(std::forward<Args>(args)...);
	return free_index;
}

// Note: MemoryHeap never really deletes anything -- it just overwrites, so
// old indices will always return a valid item wrt. memory.
template <typename T>
T& MemoryHeap<T>::get(Index index)
{
	assert(index < memory.size());
	return memory[index];
}

template <typename T>
const T& MemoryHeap<T>::get(Index index) const
{
	assert(index < memory.size());
	return memory[index];
}

template <typename T>
void MemoryHeap<T>::remove(Index index)
{
	free_indices.push_back(index);
}

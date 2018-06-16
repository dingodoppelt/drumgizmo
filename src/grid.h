/* -*- Mode: c++ -*- */
/***************************************************************************
 *            grid.h
 *
 *  Sat Jun 16 11:52:08 CEST 2018
 *  Copyright 2018 André Nusser
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

template <typename T>
class Grid
{
public:
	using Index = std::size_t;
	struct Pos
	{
		T x, y;

		Pos(T x, T y) : x(x), y(y) {}
	};

	Grid() = default;
	Grid(Index width, Index height);
	Grid(Index width, Index height, T value);

	bool is_valid(Index x, Index y) const;
	bool is_valid(Pos pos) const;

	Index width() const;
	Index height() const;
	T operator()(Pos pos) const;
	T operator()(Index x, Index y) const;
	T& operator()(Pos pos);
	T& operator()(Index x, Index y);

	void resize(Index width, Index height);
	void assign(Index width, Index height, T value);

private:
	Index _width;
	Index _height;
	std::vector<T> _entries;

};

template <typename T>
Grid<T>::Grid(Index width, Index height)
	: _width(width), _height(height), _entries(_width*_height) {}

template <typename T>
Grid<T>::Grid(Index width, Index height, T value)
	: _width(width), _height(height), _entries(_width*_height, value) {}

template <typename T>
bool Grid<T>::is_valid(Index x, Index y) const
{
	return x >= 0 && x < _width && y >= 0 && y < _height;
}

template <typename T>
bool Grid<T>::is_valid(Pos pos) const
{
	return is_valid(pos.x, pos.y);
}

template <typename T>
auto Grid<T>::width() const -> Index
{
	return _width;
}
	
template <typename T>
auto Grid<T>::height() const -> Index
{
	return _height;
}

template <typename T>
T Grid<T>::operator()(Pos pos) const
{
	return _entries.at(pos.x + _width*pos.y);
}

template <typename T>
T Grid<T>::operator()(Index x, Index y) const
{
	return _entries.at(x + _width*y);
}

template <typename T>
T& Grid<T>::operator()(Pos pos)
{
	return _entries.at(pos.x + _width*pos.y);
}

template <typename T>
T& Grid<T>::operator()(Index x, Index y)
{
	return _entries.at(x + _width*y);
}

template <typename T>
void Grid<T>::resize(Index width, Index height)
{
	_width = width;
	_height = height;
	_entries.resize(_width*_height);
}

template <typename T>
void Grid<T>::assign(Index width, Index height, T value)
{
	_width = width;
	_height = height;
	_entries.assign(_width*_height, value);
}

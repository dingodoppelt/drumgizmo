/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            range.h
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

// Having the iterator as template parameter enables us to also
// define ContainerRanges with const_iterators.
template <typename T, typename iterator = typename T::iterator>
struct ContainerRange
{
	ContainerRange(iterator begin, iterator end)
		: _begin(begin), _end(end) {};

	iterator begin() const { return _begin; };
	iterator end() const { return _end; };

private:
	iterator const _begin;
	iterator const _end;
};

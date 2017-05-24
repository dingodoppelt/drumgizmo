/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            layout.h
 *
 *  Sat Mar 21 15:12:36 CET 2015
 *  Copyright 2015 Bent Bisballe Nyeng
 *  deva@aasimon.org
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

#include <cstdlib>
#include <list>
#include <unordered_map>

#include <notifier.h>

namespace GUI
{

class Layout;

class LayoutItem
{
public:
	LayoutItem();
	virtual ~LayoutItem();

	void setLayoutParent(Layout* parent);

	virtual void resize(std::size_t width, std::size_t height) = 0;
	virtual void move(int x, int y) = 0;
	virtual int x() const = 0;
	virtual int y() const = 0;
	virtual std::size_t width() const = 0;
	virtual std::size_t height() const = 0;

private:
	Layout* parent;
};

//! \brief Abtract Layout class.
class Layout : public Listener
{
public:
	Layout(LayoutItem* parent);
	virtual ~Layout()
	{
	}

	virtual void addItem(LayoutItem* item);
	virtual void removeItem(LayoutItem* item);

	//! \brief Reimplement this method to create a new Layout rule.
	virtual void layout() = 0;

protected:
	void sizeChanged(int width, int height);

	LayoutItem* parent;
	typedef std::list<LayoutItem*> LayoutItemList;
	LayoutItemList items;
};

//! \brief Abstract box layout
class BoxLayout : public Layout
{
public:
	BoxLayout(LayoutItem* parent);

	//! \brief Set to false to only move the items, not scale them.
	void setResizeChildren(bool resize_children);

	void setSpacing(size_t spacing);

	// From Layout:
	virtual void layout() override = 0;

protected:
	bool resizeChildren{false};
	size_t spacing{0};
};

enum class HAlignment
{
	left,
	center,
	right,
};

//! \brief A Layout that lays out its elements vertically.
class VBoxLayout : public BoxLayout
{
public:
	VBoxLayout(LayoutItem* parent);

	void setHAlignment(HAlignment alignment);

	// From BoxLayout:
	virtual void layout() override;

protected:
	HAlignment align;
};

enum class VAlignment
{
	top,
	center,
	bottom,
};

//! \brief A Layout that lays out its elements vertically.
class HBoxLayout : public BoxLayout
{
public:
	HBoxLayout(LayoutItem* parent);

	void setVAlignment(VAlignment alignment);

	// From BoxLayout:
	virtual void layout() override;

protected:
	VAlignment align;
};

//! \brief A Layout class which places the items in a regular grid. An item can
//! span multiple rows/columns.
class GridLayout : public BoxLayout
{
public:
	// The range is open, i.e. end is one past the last one.
	struct GridRange
	{
		int column_begin;
		int column_end;
		int row_begin;
		int row_end;
	};

	GridLayout(LayoutItem* parent, std::size_t number_of_columns,
	    std::size_t number_of_rows);

	virtual ~GridLayout()
	{
	}

	// From Layout:
	virtual void removeItem(LayoutItem* item);
	virtual void layout();

	void setPosition(LayoutItem* item, GridRange const& range);

	int lastUsedRow(int column) const;
	int lastUsedColumn(int row) const;

protected:
	std::size_t number_of_columns;
	std::size_t number_of_rows;

	// Note: Yes, this is somewhat redundant to the LayoutItemList of the Layout
	// class. However, this was the best idea I had such that I could still
	// derive from Layout. If you find this ugly, feel free to fix it.
	std::unordered_map<LayoutItem*, GridRange> grid_ranges;

private:
	struct CellSize {
		std::size_t width;
		std::size_t height;
	};

	CellSize calculateCellSize() const;
	void moveAndResize(
	    LayoutItem& item, GridRange const& range, CellSize cell_size) const;
};

} // GUI::

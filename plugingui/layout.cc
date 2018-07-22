/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            layout.cc
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
#include "layout.h"

#include "widget.h"

#include <algorithm>

namespace GUI
{

LayoutItem::LayoutItem() : parent(nullptr)
{
}

LayoutItem::~LayoutItem()
{
	setLayoutParent(nullptr); // Will disconnect from layout if any.
}

void LayoutItem::setLayoutParent(Layout* p)
{
	if(this->parent)
	{
		this->parent->removeItem(this);
	}

	this->parent = p;
}

Layout::Layout(LayoutItem* parent) : parent(parent)
{
	auto widget = dynamic_cast<Widget*>(parent);
	if(widget)
	{
		CONNECT(widget, sizeChangeNotifier, this, &Layout::sizeChanged);
	}
}

void Layout::addItem(LayoutItem* item)
{
	items.push_back(item);
	item->setLayoutParent(this);
	layout();
}

void Layout::removeItem(LayoutItem* item)
{
	auto new_end = std::remove(items.begin(), items.end(), item);
	items.erase(new_end, items.end());

	layout();
}

void Layout::sizeChanged(int width, int height)
{
	layout();
}

//
// BoxLayout
//

BoxLayout::BoxLayout(LayoutItem* parent) : Layout(parent)
{
}

void BoxLayout::setResizeChildren(bool resizeChildren)
{
	this->resizeChildren = resizeChildren;
	layout();
}

void BoxLayout::setSpacing(size_t spacing)
{
	this->spacing = spacing;
	layout();
}

//
// VBoxLayout
//

VBoxLayout::VBoxLayout(LayoutItem* parent)
	: BoxLayout(parent)
	, align(HAlignment::center)
{
}

void VBoxLayout::layout()
{
	size_t y = 0;
	size_t w = parent->width();
	// size_t h = parent->height() / items.size();

	LayoutItemList::iterator i = items.begin();
	while(i != items.end())
	{
		LayoutItem* item = *i;

		if(resizeChildren)
		{
			auto num_items = items.size();
			auto empty_space = (num_items - 1) * spacing;
			auto available_space = parent->height();

			if(available_space >= empty_space)
			{
				auto item_height = (available_space - empty_space) / num_items;
				item->resize(w, item_height);
			}
			else
			{
				// TODO: Should this case be handled differently?
				item->resize(w, 0);
			}
		}

		size_t x = 0;
		switch(align)
		{
		case HAlignment::left:
			x = 0;
			break;
		case HAlignment::center:
			x = (w / 2) - (item->width() / 2);
			break;
		case HAlignment::right:
			x = w - item->width();
			break;
		}

		item->move(x, y);
		y += item->height() + spacing;
		++i;
	}
}

void VBoxLayout::setHAlignment(HAlignment alignment)
{
	align = alignment;
}

//
// HBoxLayout
//

HBoxLayout::HBoxLayout(LayoutItem* parent)
    : BoxLayout(parent), align(VAlignment::center)
{
}

void HBoxLayout::layout()
{
	if(items.empty())
	{
		return;
	}

	//	size_t w = parent->width() / items.size();
	size_t h = parent->height();
	size_t x = 0;

	LayoutItemList::iterator i = items.begin();
	while(i != items.end())
	{
		LayoutItem* item = *i;
		if(resizeChildren)
		{
			auto num_items = items.size();
			auto empty_space = (num_items - 1) * spacing;
			auto available_space = parent->width();

			if(available_space >= empty_space)
			{
				auto item_width = (available_space - empty_space) / num_items;
				item->resize(item_width, h);
			}
			else
			{
				// TODO: Should this case be handled differently?
				item->resize(0, h);
			}

			item->move(x, 0);
		}
		else
		{
			size_t y = 0;
			switch(align)
			{
			case VAlignment::top:
				y = 0;
				break;
			case VAlignment::center:
				y = (h / 2) - (item->height() / 2);
				break;
			case VAlignment::bottom:
				y = h - item->height();
				break;
			}

			int diff = 0; // w - item->width();
			item->move(x + diff / 2, y);
		}
		x += item->width() + spacing;
		++i;
	}
}

void HBoxLayout::setVAlignment(VAlignment alignment)
{
	align = alignment;
}

//
// GridLayout
//

GridLayout::GridLayout(LayoutItem* parent, std::size_t number_of_columns,
    std::size_t number_of_rows)
    : BoxLayout(parent)
    , number_of_columns(number_of_columns)
    , number_of_rows(number_of_rows)
{
}

void GridLayout::removeItem(LayoutItem* item)
{
	// manually remove from grid_ranges as remove_if doesn't work on an
	// unordered_map.
	auto it = grid_ranges.begin();
	while(it != grid_ranges.end())
	{
		if(it->first == item)
		{
			it = grid_ranges.erase(it);
		}
		else
		{
			++it;
		}
	}

	Layout::removeItem(item);
}

void GridLayout::layout()
{
	if(grid_ranges.empty())
	{
		return;
	}

	// Calculate cell sizes
	auto cell_size = calculateCellSize();

	for(auto const& pair : grid_ranges)
	{
		auto& item = *pair.first;
		auto const& range = pair.second;

		moveAndResize(item, range, cell_size);
	}
}

void GridLayout::setPosition(LayoutItem* item, GridRange const& range)
{
	grid_ranges[item] = range;
}

int GridLayout::lastUsedRow(int column) const
{
	int last_row = -1;

	for (auto const& grid_range : grid_ranges) {
		auto const& range = grid_range.second;
		if (column >= range.column_begin && column < range.column_end) {
			last_row = std::max(last_row, range.row_end - 1);
		}
	}

	return last_row;
}

int GridLayout::lastUsedColumn(int row) const
{
	int last_column = -1;

	for (auto const& grid_range : grid_ranges) {
		auto const& range = grid_range.second;
		if (row >= range.row_begin && row < range.row_end) {
			last_column = std::max(last_column, range.column_end - 1);
		}
	}

	return last_column;

}

auto GridLayout::calculateCellSize() const -> CellSize
{
	auto empty_width = (number_of_columns - 1) * spacing;
	auto available_width = parent->width();
	auto empty_height = (number_of_rows - 1) * spacing;
	auto available_height = parent->height();

	CellSize cell_size;
	if(available_width > empty_width && available_height > empty_height)
	{
		cell_size.width = (available_width - empty_width) / number_of_columns;
		cell_size.height = (available_height - empty_height) / number_of_rows;
	}
	else
	{
		cell_size.width = 0;
		cell_size.height = 0;
	}

	return cell_size;
}

void GridLayout::moveAndResize(
	LayoutItem& item, GridRange const& range, CellSize cell_size) const
{
	std::size_t x = range.column_begin * (cell_size.width + spacing);
	std::size_t y = range.row_begin * (cell_size.height + spacing);

	std::size_t column_count = (range.column_end - range.column_begin);
	std::size_t row_count = (range.row_end - range.row_begin);
	std::size_t width = column_count * (cell_size.width + spacing) - spacing;
	std::size_t height = row_count * (cell_size.height + spacing) - spacing;

	if(resizeChildren)
	{
		item.move(x, y);

		if(cell_size.width * cell_size.height != 0)
		{
			item.resize(width, height);
		}
		else
		{
			item.resize(0, 0);
		}
	}
	else
	{
		auto x_new = (item.width() > width) ? x : x + (width - item.width()) / 2;
		auto y_new = (item.height() > height) ? y : y + (height - item.height()) / 2;

		item.move(x_new, y_new);
	}
}

} // GUI::

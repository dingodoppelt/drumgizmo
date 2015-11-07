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
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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

#include <list>
#include <cstdlib>

namespace GUI {

class Layout;

class LayoutItem {
public:
	LayoutItem();
	virtual ~LayoutItem();

	void setLayoutParent(Layout* parent);

	virtual void resize(int width, int height) = 0;
	virtual void move(size_t x, size_t y) = 0;
	virtual size_t x() = 0;
	virtual size_t y() = 0;
	virtual size_t width() = 0;
	virtual size_t height() = 0;

private:
	Layout* parent;
};

//! \brief Abtract Layout class.
class Layout
{
public:
	Layout(LayoutItem *parent);
	virtual ~Layout() {}

	virtual void addItem(LayoutItem *item);
	virtual void removeItem(LayoutItem *item);

	//! \brief Reimplement this method to create a new Layout rule.
	virtual void layout() = 0;

protected:
	LayoutItem *parent;
	typedef std::list<LayoutItem *> LayoutItemList;
	LayoutItemList items;
};

//! \brief Abstract box layout
class BoxLayout : public Layout {
public:
	BoxLayout(LayoutItem *parent);

	//! \brief Set to false to only move the items, not scale them.
	void setResizeChildren(bool resize_children);

	// From Layout:
	virtual void layout() override  = 0;

protected:
	bool resize_children;
};

//! \brief A Layout that lays out its elements vertically.
class VBoxLayout : public BoxLayout {
public:
	VBoxLayout(LayoutItem *parent);

	typedef enum {
		HALIGN_LEFT,
		HALIGN_CENTER,
		HALIGN_RIGHT,
	} alignment_t;

	void setHAlignment(alignment_t alignment);

	// From BoxLayout:
	virtual void layout() override;

protected:
	alignment_t align;
};

//! \brief A Layout that lays out its elements vertically.
class HBoxLayout : public BoxLayout {
public:
	HBoxLayout(LayoutItem *parent);

	typedef enum {
		VALIGN_TOP,
		VALIGN_CENTER,
		VALIGN_BOTTOM,
	} alignment_t;

	void setVAlignment(alignment_t alignment);

	// From BoxLayout:
	virtual void layout() override;

protected:
	alignment_t align;
};

} // GUI::

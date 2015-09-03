/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            checkbox.h
 *
 *  Sat Nov 26 15:07:44 CET 2011
 *  Copyright 2011 Bent Bisballe Nyeng
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

#include "widget.h"
#include "image.h"
#include "notifier.h"

namespace GUI {

class CheckBox : public Widget {
public:
  CheckBox(Widget *parent);

  void setText(std::string text);

  bool isFocusable() override { return true; }

  bool checked();
  void setChecked(bool checked);

  Notifier<bool> stateChangedNotifier;

protected:
  virtual void clicked() {}

  // From Widget:
  virtual void repaintEvent(RepaintEvent *e) override;
  virtual void buttonEvent(ButtonEvent *e) override;
  virtual void keyEvent(KeyEvent *e) override;

private:
  void internalSetChecked(bool checked);

  Image bg_on;
  Image bg_off;
  Image knob;

  bool state;
  bool middle;

  std::string _text;
};

} // GUI::

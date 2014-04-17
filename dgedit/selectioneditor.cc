/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            selectioneditor.cc
 *
 *  Thu Apr 17 17:25:18 CEST 2014
 *  Copyright 2014 Bent Bisballe Nyeng
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
#include "selectioneditor.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

QLineEdit *createWidget(QString name, QWidget *parent)
{
  QHBoxLayout *l = new QHBoxLayout();

  l->addWidget(new QLabel(name));
  
  QLineEdit *edt = new QLineEdit();
  QObject::connect(edt, SIGNAL(editingFinished()),
                   parent, SLOT(updateSelection()));

  l->addWidget(edt);

  ((QHBoxLayout *)parent->layout())->addLayout(l);
  
  return edt;
}

SelectionEditor::SelectionEditor(Selections &s)
  : selections(s)
{
  cur = SEL_NONE;

  setLayout(new QVBoxLayout());

  from = createWidget("From:", this);
  to = createWidget("To:", this);
  fadein = createWidget("FadeIn:", this);
  fadeout = createWidget("FadeOut:", this);
  energy = createWidget("Energy:", this);
  name = createWidget("Name:", this);
}

void SelectionEditor::updateSelection()
{
  Selection sel;

  sel.from = from->text().toInt();
  sel.to = to->text().toInt();
  sel.fadein = fadein->text().toInt();
  sel.fadeout = fadeout->text().toInt();
  sel.energy = energy->text().toDouble();
  sel.name = name->text();

  selections.update(cur, sel);
}

void SelectionEditor::update()
{
  Selection sel = selections.get(cur);
  from->setText(QString::number(sel.from));
  to->setText(QString::number(sel.to));
  fadein->setText(QString::number(sel.fadein));
  fadeout->setText(QString::number(sel.fadeout));
  energy->setText(QString::number(sel.energy));
  name->setText(sel.name);
}

void SelectionEditor::added(sel_id_t id)
{
  // Nothing to do here...
}

void SelectionEditor::updated(sel_id_t id)
{
  if(cur == id) {
    update();
  }
}

void SelectionEditor::removed(sel_id_t id)
{
  if(cur == id) {
    cur = SEL_NONE;
    update();
  }
}

void SelectionEditor::activeChanged(sel_id_t id)
{
  cur = id;
  update();
}

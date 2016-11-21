/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            generator.cc
 *
 *  Mon Nov 21 20:16:00 CEST 2016
 *  Copyright 2016 Lars Muldjord
 *  muldjordlars@gmail.com
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

#include "generator.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QFont>

Generator::Generator()
{
  QLabel *fontLabel = new QLabel("Font family:");
  fontLineEdit = new QLineEdit("DejaVu");
  QLabel *sizeLabel = new QLabel("Font px size:");
  sizeLineEdit = new QLineEdit("12");
  QLabel *outputLabel = new QLabel("Output file:");
  outputLineEdit = new QLineEdit("output");

  QVBoxLayout *layout = new QVBoxLayout();
  QPushButton *renderButton = new QPushButton("Generate");
  connect(renderButton, SIGNAL(clicked()), this, SLOT(initGenerate()));

  layout->addWidget(fontLabel);
  layout->addWidget(fontLineEdit);
  layout->addWidget(sizeLabel);
  layout->addWidget(sizeLineEdit);
  layout->addWidget(outputLabel);
  layout->addWidget(outputLineEdit);
  layout->addWidget(renderButton);

  setLayout(layout);
}

Generator::~Generator()
{
}

void Generator::initGenerate()
{
  qDebug("Generating font to file '%s' using font '%s' at size '%s'...",
         outputLineEdit->text().toStdString().c_str(),
         fontLineEdit->text().toStdString().c_str(),
         sizeLineEdit->text().toStdString().c_str());

  QImage image(500, 16, QImage::Format_ARGB32);
  image.fill(Qt::transparent);

  QFont font;
  font.setFamily(fontLineEdit->text());
  font.setPixelSize(sizeLineEdit->text().toInt());

  QPainter p(&image);
  p.setPen(QPen(QColor(0, 0, 0, 255)));
  p.setFont(font);

  p.drawText(0, 10, "AjEg");
  
  image.save(outputLineEdit->text() + ".png");
}

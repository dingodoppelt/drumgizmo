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

#define ALPHATHRES 150

#define FONTALPHA 80
#define EMBOSSALPHA 230

Generator::Generator()
{
  QLabel *fontLabel = new QLabel("Font family:");
  fontLineEdit = new QLineEdit("Arial");
  QLabel *sizeLabel = new QLabel("Font px size:");
  sizeLineEdit = new QLineEdit("13");
  QLabel *outputLabel = new QLabel("Output file:");
  outputLineEdit = new QLineEdit("../../plugingui/resources/font");
  embossEnabled = new QCheckBox("Apply embossing");

  QVBoxLayout *layout = new QVBoxLayout();
  QPushButton *renderButton = new QPushButton("Generate");
  connect(renderButton, SIGNAL(clicked()), this, SLOT(initGenerate()));

  layout->addWidget(fontLabel);
  layout->addWidget(fontLineEdit);
  layout->addWidget(sizeLabel);
  layout->addWidget(sizeLineEdit);
  layout->addWidget(embossEnabled);
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

  QList<QImage> chars;

  maxSize = 100;

  int vertOffset = 0;
  int fontHeight = maxSize;
  
  font.setFamily(fontLineEdit->text());
  font.setPixelSize(sizeLineEdit->text().toInt());
  font.setStretch(97);
  font.setHintingPreference(QFont::PreferVerticalHinting);
  
  setVertLimits(vertOffset, fontHeight);
  
  for(int a = 0; a <= 255; ++a) {
    int horizOffset = 0;
    int charWidth = 0;
    QImage image(maxSize, maxSize, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    if(a < 32 || (a > 126 && a < 160)) {
      horizOffset = 0;
      charWidth = 0;
    } else {
      setHorizLimits(horizOffset, charWidth, a, image);
    }
    if(a == 32) {
      horizOffset = 0;
      charWidth = sizeLineEdit->text().toInt() / 8;
    }
    QImage finalChar(image.copy(horizOffset, vertOffset, charWidth + 2, fontHeight));
    QPainter p(&finalChar);
    p.setPen(QPen(QColor(255, 0, 255)));
    p.drawLine(finalChar.width() - 1, 0, finalChar.width() - 1, finalChar.height());
    chars.append(finalChar);
  }

  assembleFinalFont(chars, fontHeight);
}

void Generator::setVertLimits(int &vertOffset, int &fontHeight)
{
  QImage image(maxSize, maxSize, QImage::Format_ARGB32);
  image.fill(Qt::transparent);
    
  QPainter p(&image);
  p.setPen(QPen(QColor(0, 0, 0, 255)));
  p.setFont(font);
    
  // Render all chars to give us a maximum overall font height
  for(int a = 0; a <= 255; ++a) {
    if(embossEnabled->isChecked()) {
      p.setPen(QPen(QColor(255, 255, 255, EMBOSSALPHA)));
      p.drawText(maxSize / 2, maxSize / 2 + 1, QChar(a));
      p.setPen(QPen(QColor(0, 0, 0, 255)));
    }
    p.drawText(maxSize / 2, maxSize / 2, QChar(a));
  }

  vertOffset = getVertOffset(image);
  fontHeight = getFontHeight(image, vertOffset);
}

void Generator::setHorizLimits(int &horizOffset, int &charWidth,
                               const int &curChar, QImage &image)
{
  QPainter p(&image);
  p.setPen(QPen(QColor(0, 0, 0, 255)));
  p.setFont(font);
    
  if(embossEnabled->isChecked()) {
    p.setPen(QPen(QColor(255, 255, 255, EMBOSSALPHA)));
    p.drawText(maxSize / 2, maxSize / 2 + 1, QChar(curChar));
    p.setPen(QPen(QColor(0, 0, 0, 255)));
  }
  // Draw twice to make it clearer
  p.drawText(maxSize / 2, maxSize / 2, QChar(curChar));
  p.setPen(QPen(QColor(0, 0, 0, FONTALPHA)));
  p.drawText(maxSize / 2, maxSize / 2, QChar(curChar));

  horizOffset = getHorizOffset(image);
  charWidth = getFontWidth(image, horizOffset);
}

int Generator::getVertOffset(const QImage &image)
{
  for(int y = 0; y < maxSize; ++y) {
    for(int x = 0; x < maxSize; ++x) {
      if(qAlpha(image.pixel(x, y)) != 0) {
        return y;
      }
    }
  }
  return 0;
}

int Generator::getFontHeight(const QImage &image, const int &vertOffset)
{
  for(int y = maxSize - 1; y > vertOffset; --y) {
    for(int x = 0; x < maxSize; ++x) {
      if(qAlpha(image.pixel(x, y)) != 0) {
        return y + 1 - vertOffset;
      }
    }
  }
  return 0;
}

int Generator::getHorizOffset(const QImage &image)
{
  for(int x = 0; x < maxSize; ++x) {
    for(int y = 0; y < maxSize; ++y) {
      // Check for alpha threshold to make sure we don't get too much space between chars
      if(qAlpha(image.pixel(x, y)) > ALPHATHRES) {
        return x;
      }
    }
  }
  return 0;
}

int Generator::getFontWidth(const QImage &image, const int &horizOffset)
{
  for(int x = maxSize - 1; x > horizOffset; --x) {
    for(int y = 0; y < maxSize; ++y) {
      // Check for alpha threshold to make sure we don't get too much space between chars
      if(qAlpha(image.pixel(x, y)) > ALPHATHRES) {
        return x + 1 - horizOffset;
      }
    }
  }
  return 0;
}

void Generator::assembleFinalFont(const QList<QImage> &chars, const int &fontHeight)
{
  int imageWidth = 0;
  for(int a = 0; a < chars.length(); ++a) {
    imageWidth += chars.at(a).width();
  }

  QImage finalFont(imageWidth, fontHeight, QImage::Format_ARGB32);
  finalFont.fill(Qt::transparent);
  QPainter p(&finalFont);
  int curOffset = 0;
  for(int a = 0; a < chars.length(); ++a) {
    p.drawImage(curOffset, 0, chars.at(a));
    curOffset += chars.at(a).width();
  }
  finalFont.save(outputLineEdit->text() + ".png");

}
